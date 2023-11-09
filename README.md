# Ray Tracing in One Weekend

This is my implementation of [Peter Shirley's Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html#overview).  

This project was used as a way to explore the C++ language, while learning some graphics programming. I had lots of fun! 

## Final render
![fig34](images/fig34.png)  
*Very cool*

# Notes 

## vec3 class

We create a class that defines a 3-D points. We use the same class for vectors and colour (r,g,b) values since we write less code by doing this. Nonetheless, we create aliases `point3` and `color3` to make the code more readable.

## Rays, camera and background

### Ray definition
We need to define the notion of a ray and what colour is seen along that ray.

A ray is of the form $P(t)=A+tb$. A is the origin point, b is the direction vector. For t > 0, we get points along P(t) in front of A, for negative t, we get values behind A.

The ray class is defined to store the origin and direction vector, and be able to compute `P(t)` 

### Sending rays into a scene

A raytracer sends rays through pixels, and computes the color seen in the direction of these rays. 
Works in these 3 steps:
	- Send rays from 'eye' (camera) through the pixel
	- Determine which objects the ray intersects
	- Compute the color of the closest intersection point

We need to set up pixel dimensions for our image. So lets say we desire our image to have an aspect ratio of 16:9. This means the width : height ratio is 16:9. We can define a width, then use aspect ratio to calculate height, which we ensure is > 1.

We also need to define a viewport, which is a virtual rectangle in 3-D space that contains a grid of all image pixel positions. 

Finally, we set the camera centre to be at point (0,0,0), the x-axis to be to the right, y-axis up, and negative z-axis towards the viewing direction.

![img](images/img.png)

Remember that we want to render the image from the zeroth pixel in the top-left corner, then move row by row until we render the pixel in the bottom-right corner. This means that the y-axis is increasing going down, which doesn't follow the convention we have defined above. We define a vector `V_u` which runs from the left to the right, and another vector `V_v` which runs from the top to the bottom. We also calculate pixel deltas. 

![fig4](images/fig4.png)

To render a sky-like scene, we need to render a gradient that blends white and blue, based on the height of the y-coordinate, after making the ray direction a unit-vector.

We use linear interpolation to blend blue and white. 

![fig5](images/fig5.png)  
*Blue to white gradient based on y-coordinate of ray*

## Adding spheres

The equation of a sphere is $x^2+y^2+z^2=r^2$. This can also be interpreted as saying that if a point (x,y,z) is on a sphere, then $x^2+y^2+z^2=r^2$, if the point is in the sphere then $x^2+y^2+z^2<r^2$ if $x^2+y^2+z^2>r^2$

If we choose the centre of the sphere to be some arbitrary point $(C_{x}, C_{y}, C_{z})$ then the equation becomes: $(x-C_{x})^2+(y-C_{y})^2+(z-C_{z})^2=r^2$ which isn't nice to work with.

Let's represent this with vectors. Notice that the vector from the centre $C$ to a point $P$ is $P-C$. The dot product of this vector with itself is: $(P-C).(P-C)=(x-C_{x})^2+(y-C_{y})^2+(z-C_{z})^2$

For our purposes, we want to know whether our ray $P(t)$ hits the sphere at any point. So we want to find all $t$ which satisfy:
$$(P(t)-C).(P(t)-C)=r^2$$
We replace $t$ with its expanded form and solve for $t$
$$(tb+(A-C)).(tb+(A-C))=r^2$$
$$t^2b.b + 2tb.(A-C)+(A-C).(A-C)-r^2=0$$
This is a quadratic we can solve. Number of roots tells us about intersection of ray with sphere.

![fig6](images/fig6.png)

0 roots => no intersection
1 root => 1 intersection
2 roots => 2 intersections (we'll need to find the closest one to the camera)

Based on this, we can write a function which receives a ray, a sphere centre and radius, then based on the determinant, tells us whether the ray hits the sphere. We can check for this in the `ray_color` function, and have it return some color if there's is a hit. So it colours pixels that correspond to a sphere we have defined in some arbitrary centre with arbitrary radius.

![fig7](images/fig7.png)  
*Our first raytraced image*

## Surface normals and multiple objects
### Shading with surface normals

First, we decide whether to make surface normals unit length, since we will be needing them in many places, therefore we don't have to keep recalculating them each time we need them. We can sometimes be more efficient with how we generate these normals (not having to square root) by taking advantage of the specific geometry we are working with. For example, sphere surface normals can be made unit length by just dividing them by the radius. 

We define the normal pointing outward to be in the direction of the hit point minus the centre.

We can visualise these normals using a color map on the sphere. We know these will be of unit length (between -1 and 1), so we scale each x,y,z component to be between 0 and 1, then return those as rgb values. 

![fig8](images/fig8.png)  
*Shading the sphere with surface normals*

### Front faces vs back faces

Currently, our surface normals always point outwards from the surface of the object they hit. We could also change this convention, to having the rays always point against the ray. This is important because later, we will need to work out whether the ray is intersecting the object from the outside, or from the inside.

If the ray is always outward, then we can tell whether the ray intersects from the outside by doing a dot product with the surface normal, and checking whether it is negative.  

### Hittable list abstraction

We create a class that represents a list of things that can be hit. We create a hit function which goes through the vector of `hittable` objects and returns a boolean for whether or not we hit anything. There's also a hit record passed by reference, which at the end should contain the surface normal, hit point, and t of the closest object to the camera.

The ray color function now looks like this

```C++
color ray_color(ray& r, const hittable& world){
	hit_record rec;
	
	if(world.hit(r, interval(0, infinity), rec)){
		return 0.5*(rec.normal + color(1,1,1));
	}
	
	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5*(unit_direction.y() + 1.0);
	return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}
```

If we have hit anything, the `rec` variable will contain information about it's normal vector, which we use to shade it. 

If we've hit nothing, we render the sky.

![fig9](images/fig9.png)

## Anti aliasing

When we zoom into the image, we notice the edges are have this staircase effect. We are doing point sampling on each pixel, which is why we get this. We can mitigate this by averaging the color over surrounding areas of a pixel.

We modify the code to take into account the number of samples taken per pixel, such that we average out the samples before writing the color to the `ppm` file.

![fig10](images/fig10.png)  
*Anti-aliasing to produce smooth edges*

## Diffuse materials

### Simple diffuse materials

Diffuse materials that don't emit their own light take on the color of their surroundings, but modulate it with their own intrinsic color. When rays hit a diffuse material, they bounce off into random directions. Note that they may be absorbed instead of reflected - the more rays an object absorbs, the darker it is.

The simplest diffuse material is one which directs a ray in any direction away from the surface with equal probability. 
![fig11](images/fig11.png)


We need to add functionality to generate arbitrary vectors:

```C++
vec3 random_vec(){return vec3(double_random(), double_random(), double_random());}

vec3 random_vec(double min, double max){return vec3(double_random(min, max), double_random(min, max), double_random(min, max));}
```

We need to produce vectors in a that are within a hemisphere (See picture above.)
The analytical methods for doing this are tricky and quite involved, so we use a simpler method - keep generating samples until you get good samples.

Steps taken to generate random vectors within unit sphere:
1. Generate random point within unit cube, and reject it if it isn't in unit sphere 
2. Normalise vector within unit sphere 
3. Invert vector if in wrong hemisphere

If a ray keeps 100% of its color, then the object is white, if it keeps 0% of its color, then the object is black. In the demo, we have a recursive call to ray color, where we pass a ray whose origin is the hit point, in the direction of the random vector in the hemisphere. We multiply that function call with 0.5, so we want the ray to keep 50% of its color, we expect grey spheres.

![fig12](images/fig12.png)  
*Matte material grey*

![fig13](images/fig13.png)  
*Matte material black*

![fig15](images/fig15.png)  
*Matte white takes on color of surroundings*

### Fixing shadow acne

When a ray hits the sphere, it attempts to accurately find the intersection point. This causes a problem because this point may get rounded off, leading to floating point errors. This means that the centre of the ray randomly reflected off the surface will not be at the correct place (the original hit point). It may be slightly below or above the sphere. If it is below, the reflected ray intersects with the sphere surface again, giving the effect in the render above. We fix this by hacking in a lower bound for the values of t we accept as intersections. (Intersections that are too close are ignored)
```C++
if(world.hit(r, interval(0.001, infinity), rec))
```

![fig16](images/fig16.png)  
*Removal of shadow acne*

### True Lambertian reflection
We introduce a Lambertian distribution, which reflects rays with a probability proportional to $\cos(\phi)$, where $\phi$ is the angle between the ray reflected ray and the surface normal. This means that the reflected ray is more likely to scatter in a direction close to the surface normal and less likely to scatter in a direction further from the surface normal. 

We can create this distribution by adding a random unit vector to the surface normal.

At the hit point `p` there is a surface normal `n`. The sphere has 2 sides at the hit point, and therefore only 2 unique unit spheres can be tangent to the sphere at the hit point. These spheres will be displaced from the surface by their radius. There will be one sphere in the direction of `p+n` and another in the direction of `p+n`

We need to choose the sphere that is in the direction of the ray origin, then pick a random point `S` on the surface of this sphere, and send a ray from the hit point to `S`. This vector is `S-P` 

![fig17](images/fig17.png)  
*So adding a random unit vector to n gives us some vector $PS$ which is what we want

![fig18](images/fig18.png)  
*True Lambertian scattering*

Notice how the shadows are more pronounced, and the spheres have a slight blue tint from the sky?

### Using Gamma correction for correct color intensity

If we render the gamut of our renderer, we see that the color intensities aren't quite right. The image above is 50% reflective, but looks rather dark.

We are currently giving our image viewer data in the linear space (no gamma transform applied), yet it expects a gamma corrected image, hence why it appears inaccurately dark.

This should give us a more consistent ramp from darkness to lightness

![fig19](images/fig19.png)  
*Gamma corrected spheres*

## Metal

### Abstract class for materials
We start by refactoring the code to have an abstract class for all material types. We need to store information about whether a material scattered a ray, and if so, which ray it scattered. 

### Modelling light scatter and reflectance
For the diffuse materials, we could make it such that they always scatter with probability `p` then attenuate with reflectance `R`, or we could make it such that they sometimes scatter with probability `1-p`, and never attenuate. 

We choose the first option.

### Mirrored reflection
When a ray hits a polished metal surface, it won't be scattered randomly, here we have normal reflection.

![fig20](images/fig20.png)

The reflected ray is given by v + 2b.

![fig21](images/fig21.png)  
*Shiny metal*

We can have fuzzier metal surface by perturbing the reflected ray to point in a new direction.

![fig22](images/fig22.png)  
*Fuzzy metal*

## Dielectrics

Materials such as glass and water are dielectrics. When a ray falls onto them, it splits into a reflected ray and refracted ray. We handle this by randomly choosing between reflection and refraction, generating one scattered ray on each iteration.

### Snell's law
Refraction is described by $$\eta \sin(\theta)=\eta'\sin(\theta')$$
![fig23](fig23.png)  
*Geometry of Snell's law*

$\eta$ is refractive index for air, usually 1, $\eta'$ is refractive index for glass = 1.3-1.7, diamond = 2.4

![fig24](images/fig24.png)  
*A surface that always refracts*

### Total internal reflection
When the ray is in a material of higher refractive index, sometimes, there's no real solution to Snell's law.
$$\sin(\theta')=\frac{\eta}{\eta'}\sin(\theta)$$

The left hand side cannot be greater than 1, so if $\frac{\eta}{\eta'}\sin(\theta) > 1$, the material cannot refract, and should therefore reflect the ray.

$\cos(\theta)=R.n$ and $\sin(\theta)=\sqrt{ 1-\cos^2(\theta) }$ 

So we can check whether refraction is possible before performing it.

![fig25](images/fig25.png)  
*TIR check*

### Schlick approximation

Real glass has reflectivity that varies with angle. We can approximate this behaviour using a polynomial approximation by Christopher Schlick.

![fig26](images/fig26.png)  
*Reflectivity varying with angle*

## Positionable camera

We decide on using a vertical FOV. We can move the focal distance from the $z=-1$ plane to some other plane, as long as we make h **a ratio to this distance.**  

![fig27](images/fig27.png)

We see that $h=\tan\left( \frac{\theta}{2} \right)$ (Remember that $h$ is a ratio)

![fig28](images/fig28.png)  
*We see a wide angle view of 2 spheres touching each other*

## Positioning and orientating the camera

We define the point the camera is to be `lookfrom` and the point at which we are looking to be `lookat`. Next, we need to define a way to rotate the camera around the `lookfrom-lookat` axis. 

We define an up vector for the camera. We can use any up vector, so long as it isn't parallel to the direction in which we are looking. We project this vector onto the plane orthogonal to the direction in which we are looking.  We can produce an orthonormal set of vectors $(u,v,w)$ that define the camera's orientation. 

![fig29](fig29.png)

![fig30](images/fig30.png)
## Defocus blur

How can we simulate the effect of materials appearing in and out of focus based on distance from the camera. 

Why does this happen in real cameras?

Real cameras use a large hole to let in light instead of a pin hole. Leaving the design at this would defocus everything, so a lens is added in front of the film. This makes it such that there's a certain distance at which objects are in focus, and are linearly out of focus the further away from this distance they are. This distance is the *focus distance.*

Note that this is different from the *focal distance* which is the distance between the camera centre and the image plane. However, in our model, these will be the same since we place our pixel grid onto the focus plane. 

In a real camera, this focus distance is dependent on the distance between the lens and the film. 

### Thin-lens approximation

Send rays from an infinitely small lens towards the pixel of interest on the focus plane, where everything on this plane is in perfect focus. In practise, we accomplish this by placing the viewport on the focus plane.

Putting everything together:
1. The focus distance is orthogonal to the viewing direction
2. The focus distance is the distance between the camera centre and the focus plane
3. The viewport lies on the focus plane, centred on the camera view direction vector
4. Grid of pixel locations lies inside the focus plane
5. Random image sample locations are chosen from the region around the current pixel
6. Camera sends ray from a random point on the lens to the image sample location

### Generating sample rays

We will construct a disk centred at the camera centre. The larger the disk, the larger the defocus blur. 

![fig31](images/fig31.png)  
*Blur*
## Final render

Let's add a bunch of random spheres into the scene


# Plans for the future
- Adding solid textures
- Adding surface textures
- Adding multithreading support

# What's going on?

![fig32](images/fig32.png)  
*Too close to metal sphere*

![fig33](images/fig33.png)  
*Ground too small*
