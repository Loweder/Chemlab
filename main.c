#include "math.h"
#include <glad/glad.h>
#include <SDL.h>

uint32_t width = 200, height = 200;

vec3_t world_up = {0, 1, 0};

uint64_t acount = 201, bcount = 201;
float x = -50, y = -50, z = 0, stepa = 0.5, stepb = 0.5;
enum axis in_axis = AXIS_Z, out_axis = AXIS_Y;

const char vData[] = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in float in_color;\n"
"out float shader_color;\n"
"uniform mat4 matrix;"
"void main() {\n"
"  gl_Position = matrix * vec4(pos, 1.0);\n"
"  shader_color = in_color;\n"
"}\n";
const char fData[] = 
"#version 330 core\n"
"in float shader_color;\n"
"out vec4 color;\n"
"void main() {\n"
"  float normalizedValue = (clamp(shader_color, -1.0, 1.0) + 1.0) * 0.5;\n"
"  vec4 colorAtMinusOne = vec4(0.0, 0.0, 1.0, 1.0);\n"
"  vec4 colorAtZero = vec4(1.0, 1.0, 1.0, 1.0);\n"
"  vec4 colorAtOne = vec4(1.0, 0.0, 0.0, 1.0);\n"
"  if (normalizedValue < 0.5) {\n"
"      color = mix(colorAtMinusOne, colorAtZero, normalizedValue * 2.0);\n"
"  } else {\n"
"      color = mix(colorAtZero, colorAtOne, (normalizedValue - 0.5) * 2.0);\n"
"  }\n"
"}\n";

float A = 1;
vec3_t k = {1, 0, 1};
float w = 2;
vec2_t waveFunction(vec4_t pos) {
  vec3_t p3 = {pos.x, pos.y, pos.z};
  vec2_t A_c = {A, 0};
  vec2_t exp = {M_E, 0};
  vec2_t pow = {0, v_dot(k, p3) - (pos.w*w)};
  return c_mul(A_c, c_pow(exp, pow));
  //return (vec2_t){sin(pos.y*4+pos.w)*sin(pos.x*10), sin(pos.x*5+pos.w)*sin(pos.y*5)};
}

GLuint makeShader(GLenum type, const char *data) {
  GLuint result = glCreateShader(type);
  glShaderSource(result, 1, &data, NULL);
  glCompileShader(result);
  GLint status, log_size;
  glGetShaderiv(result, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &log_size);
    char *log = malloc(log_size);
    glGetShaderInfoLog(result, log_size, NULL, log);
    fprintf(stderr, "Shader log: \n%s\n", log);
    free(log);
  }
  return result;
}

GLuint makeProgram(const char *vData, const char *fData) {
  GLuint result = glCreateProgram(), vertex = makeShader(GL_VERTEX_SHADER, vData), fragment = makeShader(GL_FRAGMENT_SHADER, fData);
  glAttachShader(result, vertex);
  glAttachShader(result, fragment);
  glLinkProgram(result);
  GLint status, log_size;
  glGetProgramiv(result, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    glGetProgramiv(result, GL_INFO_LOG_LENGTH, &log_size);
    char *log = malloc(log_size);
    glGetProgramInfoLog(result, log_size, NULL, log);
    fprintf(stderr, "Program log: \n%s\n", log);
    free(log);
  }
  glDetachShader(result, vertex);
  glDetachShader(result, fragment);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return result;
}

GLushort *makeMeshIndices(void) {
  GLushort *data = malloc(sizeof(GLushort)*(acount-1)*(bcount*2));
  if (!data) return 0;
  uint64_t counter = 0;
  for (uint64_t a = 0; a < acount - 1; a++) {
    if (a % 2 == 0) {
      for (uint64_t b = 0; b < bcount; b++) {
	data[counter++] = a*bcount + b;
	data[counter++] = (a+1)*bcount + b;
      }
    } else {
      for (uint64_t b = bcount - 1; b < (1LL << 63); b--) {
	data[counter++] = (a+1)*bcount + b;
	data[counter++] = a*bcount + b;
      }
    }
  }
  return data;
}

mat4_t buildMatrix(vec3_t *front, vec3_t *pos) {
  /*mat4_t view = m_lookAt(*front, world_up, *pos), projection = m_orthogonal(30, (float) width/height, 0.1, 100);*/
  /*return m_mul(projection, view);*/
  mat4_t view = m_lookAt(*front, world_up, *pos), projection = m_perspective(1, (float) width/height, 0.1, 100);
  return m_mul(projection, view);
  return m_lookAt(*front, world_up, *pos);
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    fprintf(stderr, "Failed to initialize SDL");
    return 1;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *win = SDL_CreateWindow("Chemlab", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!win) {
    fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_GLContext ctx = SDL_GL_CreateContext(win);
  if (!ctx) {
    fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  //TODO remove
  if (0 && SDL_GL_SetSwapInterval(0) < 0) {
    fprintf(stderr, "Failed to turn off VSync: %s\n", SDL_GetError());
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 1.0);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  GLuint vao, vbo, ebo, shader, matrix, scan = 1;
  vec3_t pos = {0, 0, -2}, front = {0, 0, 1};
  mat4_t viewMatrix = buildMatrix(&front, &pos);

  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4_t)*acount*bcount, 0, GL_DYNAMIC_DRAW);

    GLushort *data = makeMeshIndices();
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*(acount-1)*bcount*2, data, GL_STATIC_DRAW);
    free(data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 16, (void*)12);
    glEnableVertexAttribArray(1);
  
    shader = makeProgram(vData, fData);
    glUseProgram(shader);
    matrix = glGetUniformLocation(shader, "matrix");
  }

  uint64_t last = SDL_GetTicks64();
	while(1) {
		uint64_t current = SDL_GetTicks64();
		float deltaTime = (current - last) / 1000.f;
		last = current;
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					goto exit;
				case SDL_WINDOWEVENT:
					switch (e.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							width = e.window.data1;
							height = e.window.data2;
							glViewport(0, 0, width, height);
							viewMatrix = buildMatrix(&front, &pos);
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					if (!scan) break;
					vec3_t right = v_cross(front, world_up);
					mat4_t pitch = m_rotate(world_up, -e.motion.xrel / 1000.f);
					mat4_t yaw = m_rotate(right, -e.motion.yrel / 1000.f);
					printf("Motion: X: %d, Y: %d\n", e.motion.xrel, e.motion.yrel);
					mat4_t rot = m_mul(yaw, pitch);
					front = v_norm(v_to3d(m_apply(rot, v_to4d(front))));
					viewMatrix = buildMatrix(&front, &pos);
					break;
				case SDL_KEYDOWN:
					{
						vec3_t right = v_cross(front, world_up);
						vec3_t n_front = v_cross(world_up, right);
						vec3_t offset = {0, 0, 0};
						switch (e.key.keysym.sym) {
							case SDLK_w:
								offset = n_front;
								break;
							case SDLK_s:
								offset = v_sub(offset, n_front);
								break;
							case SDLK_d:
								offset = right;
								break;
							case SDLK_a:
								offset = v_sub(offset, right);
								break;
							case SDLK_x:
								offset = world_up;
								break;
							case SDLK_z:
								offset = v_sub(offset, world_up);
								break;
							case SDLK_i:
								scan = 1;
								SDL_SetRelativeMouseMode(SDL_TRUE);
								break;
							case SDLK_o:
								scan = 0;
								SDL_SetRelativeMouseMode(SDL_FALSE);
								break;
							case SDLK_ESCAPE:
								SDL_Quit();
								break;
						}
						offset = v_scale(offset, deltaTime*15);
						pos = v_add(pos, offset);
						viewMatrix = buildMatrix(&front, &pos);
					}
			}
		}
		glUniformMatrix4fv(matrix, 1, GL_TRUE, viewMatrix.dim1);
		vec4_t *data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		function_mesh(x, y, z, SDL_GetTicks() / 1000.f, stepa, stepb, acount, bcount, in_axis, out_axis, waveFunction, data);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLE_STRIP, (acount-1)*bcount*2, GL_UNSIGNED_SHORT, 0);
		SDL_GL_SwapWindow(win);
	}
exit:
  return 0;
}
