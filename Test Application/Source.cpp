#include <Hauk.h>
#include "AppData.h"
#include "Sphere.h"
#include "Hitable_list.h"
#include "Ray.h"
#include "Camera.h"
#include <float.h>

#define drand48() ((double)rand()/RAND_MAX)

vec3 random_in_unit_sphere();
bool bIsDrawn;
bool bIsThreadCreated;
vec3 color(const ray& r, hitable *world);
DWORD WINAPI RenderProc(LPVOID lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCommandLine, INT nCommandShow)
{
	cAnimatedWindow *AnimatedWindow = new(cAnimatedWindow);
	bIsDrawn = FALSE;
	bIsThreadCreated = false;
	AnimatedWindow->SetVideoMode(VideoDisplayModes[CUSTOM1]);
	AnimatedWindow->SetDrawFunction(DrawProc);
	AnimatedWindow->Animate();
	delete(AnimatedWindow);


	return 0;
}

HRESULT CALLBACK DrawProc(HWND hWindowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	if(bIsThreadCreated == false)
	{
		CreateThread(NULL, 0, RenderProc, (LPVOID)lParam, 0, NULL); 
		bIsThreadCreated = true;
	}

	return  0;

}

DWORD WINAPI RenderProc(LPVOID lParam)
{

	ANIMATED_WINDOW_INFO *sAnimateWindowInfo = (ANIMATED_WINDOW_INFO*)lParam;
	cDrawingBuffer *DrawBuffer = sAnimateWindowInfo->DrawingBufferObjectAddress;

	if (!bIsDrawn)
	{
		int nx = sAnimateWindowInfo->dwWindowLengthX;
		int ny = sAnimateWindowInfo->dwWindowHeightY;
		int ns = 100;
		//int nx = 200;
		//int ny = 100;

		hitable *list[2];
		list[0] = new sphere(vec3(0, 0, -1), 0.5);
		list[1] = new sphere(vec3(0, -100.5, -1), 100);
		hitable *world = new hitable_list(list, 2);

		camera cam;
		for (int j = ny - 1; j >= 0; j--)
		{
			for (int i = 0; i < nx; i++)
			{
				vec3 col(0, 0, 0);
				for (int s = 0; s < ns; s++)
				{
					float u = float(i + drand48()) / float(nx);
					float v = float(j + drand48()) / float(ny);
					ray r = cam.get_ray(u, v);
					vec3 p = r.point_at_parameter(2.0);
					col += color(r, world);
				}

				col /= float(ns);
				int ir = int(255.99*col[0]);
				int ig = int(255.99*col[1]);
				int ib = int(255.99*col[2]);

				DrawBuffer->DrawPoint2D(i, j, RGB(ib, ig, ir));
			}
		}
		bIsDrawn = TRUE;
	}

	return 0;
}



vec3 color(const ray& r, hitable *world)
{
	hit_record rec;
	if (world->hit(r, 0.0, FLT_MAX, rec))
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5*color( ray(rec.p, target-rec.p), world);
	} else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}


vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 *vec3(drand48(), drand48(), drand48()) - vec3(1,1,1);
	}while (p.squared_length() >= 1.0);
	
	return p;

}
