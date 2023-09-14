
#include "framework.h"

// vertex shader in GLSL
const char* vertexSource = R"(
	#version 330
    precision highp float;

	uniform vec3 wLookAt, wRight, wUp;          // pos of eye

	layout(location = 0) in vec2 cCamWindowVertex;	// Attrib Array 0
	out vec3 p;

	void main() {
		gl_Position = vec4(cCamWindowVertex, 0, 1);
		p = wLookAt + wRight * cCamWindowVertex.x + wUp * cCamWindowVertex.y;
	}
)";

/*
*  A programnál az volt a célom hogy a mintára hasonlíton ezért kellet kisérleteznem a határokkal
*/

// fragment shader in GLSL
const char* fragmentSource = R"(
	#version 330
    precision highp float;
	
	// these are for the calcation
	const vec3 ks = vec3(1,1,1);
	const vec3 kd = vec3(1.5f, 0.6f, 0.5f);
	const vec3 ka = vec3(0.5f,0.5f,0.5f);
	const float shininess = 700.0f;
	const vec3 La = vec3(0.5f,0.6f,0.6f);	
	const vec3 Le = vec3(0.9f,0.9f,0.9f);
	const vec3 lightPosition = vec3(0.87f,0.87f,0.25f);
	const int maxdepth = 5;
	const float epsilon = 0.01f;
	const int objFaces =12;	

	struct Hit {
		float t;
		vec3 position, normal;
		int mat;	// material index
	};


	struct Ray {
		vec3 start, dir, weight;
	};
	
	
	uniform vec3 wEye;
	uniform vec3 v[20];
	uniform int planes[objFaces *3];
	uniform vec3 F0;
	uniform float s, c;
	void getDodcaPlane(int i,  out vec3 p, out vec3 normal)
	{
		float scale = sqrt(3);
		// point of dodecaeder
		vec3 p1 = v[planes[3 * i] - 1];
		// this for help calcolating the normalvector 
		vec3 p2 = v[planes[3 * i + 1] - 1]; 
		vec3 p3 = v[planes[3 * i + 2] - 1];
		// normarvector of the plane
		normal = cross(p2 - p1, p3 - p1);
		if (dot(p1, normal) < 0) 
		{
			normal = -normal;
		}
		p = p1 * scale;
	}
	

	Hit intersectDodeca(Ray ray, Hit hit)
	{
		for (int i = 0; i < objFaces; i++)
		{
			vec3 p1, normal;
			getDodcaPlane(i, p1, normal);
			//calculate the ray parameter
			float ti = abs(dot(normal, ray.dir)) > epsilon ? dot(p1 - ray.start, normal) / dot(normal, ray.dir) : -1;
			// looking for the best parametrs
			if (ti <= epsilon || (ti > hit.t && hit.t > 0)) continue;
			// ray intersection at plane
			vec3 intersection = ray.start + ray.dir * ti;
			bool outside = false;
			bool portal =true;
			for (int j = 0; j < objFaces; j++)
			{
	
				if (i == j) 
				{
					continue;
				}
				vec3 p2, n;
				getDodcaPlane(j, p2, n);
				// edge of the wall
				if(abs(dot(n,intersection-p2))<0.15)
				{
					portal = false;
				}
				// if the point in the wrong way
				if (dot(n, intersection - p2) > 0)
				{
					outside = true;
					break;
				}
			}
			if (!outside)
			{
				hit.t = ti;
				hit.position = intersection;
				hit.normal = normalize(normal);
				hit.mat = 1;
				if(portal == true)
				{
					hit.mat=3;
				}
			}

		}
		return hit;
	}
	
	Hit intersectThing(Ray ray, Hit hit)
	{
		// Calculate Qudratic equation
		vec3 dist = ray.start - vec3(0,0,0.1);
		float a = dot(ray.dir, ray.dir);
		float b = dot(ray.dir, dist) * 2 - 2.0*2 * ray.dir.z;
		float c = dot(dist, dist) - 2.0*2  * dist.z;
		float zmin =0;
		float zmax = 0.15;
		float normz = 1;
		float discr = b * b - 4.0f * a * c;
		// if it is not negative
		if (discr >= 0)
		{
			float sqrt_discr = sqrt(discr);
			float t1 = (-b + sqrt_discr) / 2.0f / a;
			vec3 p = ray.start + ray.dir * t1;
			//looking for the edge
			if (p.z > zmax || p.z < zmin) t1 = -1;
			float t2 = (-b - sqrt_discr) / 2.0f / a;
			p = ray.start + ray.dir * t2;
			//looking for the edge
			if (p.z > zmax || p.z < zmin) t2 = -1;
			if (t2 > 0 && (t2 < t1 || t1 < 0)) t1 = t2;
			if (t1 > 0 && (t1 < hit.t || hit.t < 0))
			{
				hit.t = t1;
				hit.position = ray.start + ray.dir * hit.t;
				hit.normal = normalize(vec3(-hit.position.x,-hit.position.y,normz));
				hit.mat = 2;
			}
		}
		return hit;

	}

	Hit firstIntersect(Ray ray)
	{
		// go to all object
		Hit bestHit;
		bestHit.t = -1;
		bestHit = intersectThing(ray, bestHit);
		bestHit = intersectDodeca(ray, bestHit);
		if (dot(ray.dir, bestHit.normal) > 0) 
		{
			bestHit.normal = bestHit.normal * (-1);
		}
		return bestHit;
	}


	
	vec3 Fresnel(vec3 F0, float cosTheta) 
	{ 
		return F0 + (vec3(1, 1, 1) - F0) * pow(cosTheta, 5);
	}
	

	 mat4 RotationMatrix(vec3 w) 
	 {

		w = normalize(w);
		return mat4(vec4(s * (1 - w.x*w.x) + w.x*w.x, w.x*w.y*(1 - s) + w.z*c, w.x*w.z*(1 - s) - w.y*c, 0),
					vec4(w.x*w.y*(1 - s) - w.z*c, s * (1 - w.y*w.y) + w.y*w.y, w.y*w.z*(1 - s) + w.x*c, 0),
					vec4(w.x*w.z*(1 - s) + w.y*c, w.y*w.z*(1 - s) - w.x*c, s * (1 - w.z*w.z) + w.z*w.z, 0),
					vec4(0,							0,					0,								1));
	 }
	
	vec3 trace(Ray ray)
	{
		vec3 outRadiance = vec3(0, 0, 0);
		// the for loop for the refflection
		for (int d = 0; d < maxdepth; d++)
		{
			Hit hit = firstIntersect(ray);
			if (hit.t < 0) break;
			// for the wall
			if (hit.mat ==1)
			{
				vec3 lightdir = normalize(lightPosition - hit.position);
				float cosTheta = dot(hit.normal, lightdir);
				if (cosTheta > 0)
				{
					vec3 LeIn = Le / dot(lightPosition - hit.position, lightPosition - hit.position);
					outRadiance += ray.weight * LeIn * kd * cosTheta;
					vec3 halfway = normalize(-ray.dir + lightdir);
					float cosDelta = dot(hit.normal, halfway);
					if(cosDelta>0)
					{ 
						outRadiance += ray.weight * LeIn * ks * pow(cosDelta, shininess);
					}
				}
				ray.weight *= ka;
				break;
			}
			//for the portal
			if(hit.mat==3)
			{
				ray.weight *= Fresnel(vec3(1,1,1), dot(-ray.dir, hit.normal));
				ray.start = hit.position + hit.normal  * epsilon;
				vec4 e1 = vec4(ray.start.x,ray.start.y,ray.start.z,1)*RotationMatrix(hit.normal);
				ray.dir = reflect(ray.dir, hit.normal);
				vec4 e2 = vec4(ray.dir.x,ray.dir.y,ray.dir.z,1)*RotationMatrix(hit.normal);
				ray.start = vec3(e1.x,e1.y,e1.z);
				ray.dir = vec3(e2.x,e2.y,e2.z);
			}
			//for the golden object
			if(hit.mat==2)
			{
				ray.weight *= Fresnel(F0, dot(-ray.dir, hit.normal));
				ray.start = hit.position + hit.normal  * epsilon;
				ray.dir = reflect(ray.dir, hit.normal);
			}
		}
		outRadiance += ray.weight * La;
		return outRadiance;

	}

	

	in vec3 p;
	out vec4 fragmentColor;

	void main()
	{
		Ray ray;
		ray.start = wEye;
		ray.dir = normalize(p-wEye);
		ray.weight = vec3(1,1,1);
		fragmentColor = vec4(trace(ray),1);
	}

)";



//Camera
struct Camera {
public:
	vec3 eye, lookat, right, pvup, rvup;
	float fov = 45 * (float)M_PI / 180;

	Camera() : eye(0, 1.7, 1.7), pvup(0, 0, 1), lookat(0, 0, 0)
	{
		set();
	}
	//the position set
	void set()
	{
		vec3 w = eye - lookat;
		float f = length(w);
		right = normalize(cross(pvup, w)) * f * tanf(fov / 2);
		rvup = normalize(cross(w, right)) * f * tanf(fov / 2);
	}
	// the animation
	void Animate(float t)
	{
		float r = sqrtf(eye.x * eye.x + eye.y * eye.y);
		eye = vec3(r * cos(t) + lookat.x, r * sin(t) + lookat.y, eye.z);
		set();
	}

};


Camera camera;
GPUProgram sha;
// for calculat the gold reflection
float F(float n, float k)
{
	return ((n - 1) * (n - 1) + k * k) / ((n + 1) * (n + 1) + k * k);
}

// This will generate the virtual world
class Picture {

	unsigned int vao, vbo;
public:
	void Build()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// the virtual world cordinate
		float vertexCoords[] = { -1,-1,1,-1,1,1,-1,1 };
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		sha.create(vertexSource, fragmentSource, "fragmentColor");
		// Dodecaeders points 
		std::vector<vec3> v{
			vec3(0,0.618f,1.618f),vec3(0,-0.618f,1.618f),vec3(0,-0.618f,-1.618f),vec3(0,0.618f,-1.618f),vec3(1.618f,0,0.618f),vec3(-1.618f,0,0.618f),
			vec3(-1.618f,0,-0.618f),vec3(1.618f,0,-0.618f),vec3(0.618f,1.618f,0),vec3(-0.618f,1.618f,0),vec3(-0.618f,-1.618f,0),vec3(0.618f,-1.618f,0),
			vec3(1,1,1),vec3(-1,1,1),vec3(-1,-1,1),vec3(1,-1,1),vec3(1,-1,-1),vec3(1,1,-1),vec3(-1,1,-1),vec3(-1,-1,-1)
		};
		// put to the fragment
		for (int i = 0; i < v.size(); i++)
		{
			sha.setUniform(v[i], "v[" + std::to_string(i) + "]");
		}
		// index of the array
		std::vector<int> planes{
			1,2,16, 1,13,9, 1,14,6, 2,15,11, 3,4,18, 3,17,12, 3,20,7, 19,10,9 ,16,12,17, 5,8,18, 14,10,19 ,6,7,20
		};
		// put to the fragment
		for (int i = 0; i < planes.size(); i++)
		{
			sha.setUniform(planes[i], "planes[" + std::to_string(i) + "]");
		}
		// The gold refraction
		sha.setUniform(vec3(F(0.17, 3.1), F(0.35, 2.7), F(1.5, 1.9)), "F0");
		// for the rotation
		sha.setUniform(cosf(72), "c");
		sha.setUniform(sinf(72), "s");
	}
	// the Draw
	void Draw()
	{
		//Give the camera parametrs to GPU
		sha.setUniform(camera.eye, "wEye");
		sha.setUniform(camera.lookat, "wLookAt");
		sha.setUniform(camera.right, "wRight");
		sha.setUniform(camera.rvup, "wUp");
		// Draw the picture
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

};




Picture* picture;
// Initialization, create an OpenGL context
void onInitialization()
{
	glViewport(0, 0, windowWidth, windowHeight);
	picture = new Picture();
	picture->Build();

}

// Window has become invalid: Redraw
void onDisplay()
{
	static int nFrames = 0;
	nFrames++;
	static long tStart = glutGet(GLUT_ELAPSED_TIME);
	long tEnd = glutGet(GLUT_ELAPSED_TIME);
	printf("%d msec\r", (tEnd - tStart) / nFrames);
	glClearColor(1.0f, 0.5f, 0.8f, 1.0f);							// background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
	picture->Draw();
	glutSwapBuffers();									// exchange the two buffers
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {

}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {

}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	// the animtaion
	float f = glutGet(GLUT_ELAPSED_TIME);
	float time = f / 1000.0f;
	camera.Animate(time);
	glutPostRedisplay();
}