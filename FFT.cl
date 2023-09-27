__kernel void fft(__global float2* a, const unsigned int n, const unsigned int step)
{
    int id = get_global_id(0);
    if (id >= n) return;

    int even_index = id * step * 2;
    int odd_index = even_index + step;

    float2 even = a[even_index];
    float2 odd = a[odd_index];

    float angle = -2.0f * M_PI_F * id / n;
    float2 twiddle = (float2)(cos(angle), sin(angle));
    float2 t = twiddle * odd;

    a[even_index] = even + t;
    a[odd_index] = even - t;
}
