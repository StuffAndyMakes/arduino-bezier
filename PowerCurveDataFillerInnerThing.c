//
//  PowerCurveDataFillerInnerThing.c
//  Power curve data smoother/filler-inner thing to make my throttle curves nicer
//  Created by andy@StuffAndyMakes.com on 9/1/14.
//
//  Very excellent Bezier Curve code translated from C# written by Tolga Birdal and graciously posted here:
//  http://www.codeproject.com/Articles/25237/Bezier-Curves-Made-Simple
//

#include <stdio.h>
#include <math.h>

 
#define ARRAY_SIZE( array ) sizeof( array ) / sizeof( array[0] )
 

double factorialLookup[33] = {
    1.0,
    1.0,
    2.0,
    6.0,
    24.0,
    120.0,
    720.0,
    5040.0,
    40320.0,
    362880.0,
    3628800.0,
    39916800.0,
    479001600.0,
    6227020800.0,
    87178291200.0,
    1307674368000.0,
    20922789888000.0,
    355687428096000.0,
    6402373705728000.0,
    121645100408832000.0,
    2432902008176640000.0,
    51090942171709440000.0,
    1124000727777607680000.0,
    25852016738884976640000.0,
    620448401733239439360000.0,
    15511210043330985984000000.0,
    403291461126605635584000000.0,
    10888869450418352160768000000.0,
    304888344611713860501504000000.0,
    8841761993739701954543616000000.0,
    265252859812191058636308480000000.0,
    8222838654177922817725562880000000.0,
    263130836933693530167218012160000000.0
};
 
// just check if n is appropriate, then return the result
double factorial( int n ) {
 
    if ( n < 0 ) { printf( "ERROR: n is less than 0\n" ); }
    if ( n > 32 ) { printf( "ERROR: n is greater than 32\n" ); }
    
    return factorialLookup[n]; /* returns the value n! as a SUMORealing point number */
}
 
double Ni( int n, int i ) {
    double ni;
    double a1 = factorial( n );
    double a2 = factorial( i );
    double a3 = factorial( n - i );
    ni =  a1 / ( a2 * a3 );
    return ni;
}
 
// Calculate Bernstein basis
double Bernstein( int n, int i, double t ) {
    double basis;
    double ti; /* t^i */
    double tni; /* (1 - t)^i */
    
    /* Prevent problems with pow */
    
    if ( t == 0.0 && i == 0 ) {
        ti = 1.0;
    } else {
        ti = pow( t, i );
    }
    if ( n == i && t == 1.0 ) {
        tni = 1.0;
    } else {
        tni = pow( ( 1 - t ), ( n - i ) );
    }
    //Bernstein basis
    basis = Ni( n, i ) * ti * tni;
    return basis;
}
 
void Bezier2D( double b[], int bCount, int cpts, double p[] ) {
    int npts = bCount / 2;
    int icount, jcount;
    double step, t;
    
    // Calculate points on curve
    
    icount = 0;
    t = 0;
    step = (double)1.0 / ( cpts - 1 );
    
    for ( int i1 = 0; i1 != cpts; i1++ ) {
        if ((1.0 - t) < 5e-6) {
            t = 1.0;
        }
        jcount = 0;
        p[icount] = 0.0;
        p[icount + 1] = 0.0;
        for ( int i = 0; i != npts; i++ ) {
            double basis = Bernstein(npts - 1, i, t);
            p[icount] += basis * b[jcount];
            p[icount + 1] += basis * b[jcount + 1];
            jcount = jcount +2;
        }
        
        icount += 2;
        t += step;
    }
}
 

int main( int argc, const char * argv[] ) {
 
    double outputs[2048];
    // advanced curve
    double inputs[66] = {
        0, 0.0,
        32, 9.0,
        64, 16.0,
        96, 22.0,
        128, 28.0,
        160, 0.0,
        192, 41.0,
        224, 46.0,
        256, 50.0,
        288, 56.0,
        320, 59.0,
        352, 60.0,
        384, 61.0,
        416, 62.0,
        448, 63.0,
        480, 63.0,
        512, 64.0,
        544, 64.0,
        576, 65.0,
        608, 65.0,
        640, 66.0,
        672, 67.0,
        704, 69.0,
        736, 71.0,
        768, 75.0,
        800, 82.0,
        832, 87.0,
        864, 92.0,
        896, 98.0,
        928, 105.0,
        960, 111.0,
        992, 119.0,
        1023, 128.0
    };
    Bezier2D( inputs, ARRAY_SIZE( inputs ), 1024, outputs );
    for ( int x = 0; x < 2048; x += 2 ) {
        printf( "%li\t%li\n", (long)outputs[x], (long)outputs[x+1] );
    }
 
    return 0;
}
