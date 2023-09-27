Certainly! The reason a quadratic equation appears in this context is because we're dealing with a geometric problem that can potentially have two intersection points. When a ray intersects a sphere, it can either touch the sphere at one point, pass through the sphere hitting it at two points, or not intersect the sphere at all. Quadratic equations naturally arise in such scenarios because they can have zero, one, or two real solutions, which correspond to these geometric possibilities.

Let's go step-by-step through the formula manipulation:

### Step 1: Sphere and Ray Equations
We start with the equation of the sphere and the equation of the ray:

\[
\left\Vert \mathbf {x} -\mathbf {c} \right\Vert ^{2}=r^{2} \quad \text{(Sphere)}
\]
\[
\mathbf {x} =\mathbf {s} + t \mathbf {d} \quad \text{(Ray)}
\]

### Step 2: Finding the Intersection
We want to find the intersection, so we set the two equations equal to each other:

\[
\left\Vert \mathbf {s} + t \mathbf {d} - \mathbf {c} \right\Vert ^{2} = r^{2}
\]

### Step 3: Simplification with \( \mathbf{v} \)
For simplification, we let \( \mathbf{v} = \mathbf{s} - \mathbf{c} \), which gives:

\[
\left\Vert \mathbf {v} + t \mathbf {d} \right\Vert ^{2} = r^{2}
\]

### Step 4: Expanding the Norm
We expand the square of the norm, leading to:

\[
\mathbf{v}^2 + t^2 \mathbf{d}^2 + 2 \mathbf{v} \cdot (t \mathbf{d}) = r^2
\]

### Step 5: Further Simplification
Since \( \mathbf{d} \) is a unit vector, \( \mathbf{d}^2 = 1 \). This allows us to simplify the equation to:

\[
t^2 + 2 (\mathbf{v} \cdot \mathbf{d}) t + (\mathbf{v}^2 - r^2) = 0
\]

### Step 6: Solving the Quadratic Equation
Finally, we solve for \( t \) using the quadratic formula:

\[
t = - (\mathbf{v} \cdot \mathbf{d}) \pm \sqrt{(\mathbf{v} \cdot \mathbf{d})^2 - (\mathbf{v}^2 - r^2)}
\]

This equation gives us two possible values for \( t \), which are the two points where the ray may intersect the sphere. If the term under the square root is negative, then the ray doesn't intersect the sphere. If one or both of the \( t \) values are positive, those are the points of intersection.

I hope this breakdown clarifies why the quadratic equation is used and what each step is doing! Would you like to know more about any specific part?