#include "Arduino.h"
#include <math.h>

class BezierThrottleControl {

    static const uint16_t THROTTLE_MAP_COUNT = 256;
    uint8_t throttleCurve[THROTTLE_MAP_COUNT];
    static const float factorialLookup[33] = {
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
    uint8_t leftPin = A0, rightPin = A1;
    uint8_t minPower = 1, maxPower = 127;
    int lastCmdA = 0, lastCmdB = 0;

    float factorial( int n );
    float Ni( int n, int i );
    float Bernstein( int n, int i, float t );
    void Bezier2D( float b[], int bCount, int cpts, float p[] );

public:

    BezierThrottleControl( uint8_t minPower, uint8_t maxPower, uint8_t lp, uint8_t rp );
    void setThrottleRange( uint8_t minPower, uint8_t maxPower );
    void loop();

};

BezierThrottleControl::BezierThrottleControl( uint8_t np, uint8_t xp, uint8_t lp, uint8_t rp ) {
    leftPin = lp;
    rightPin = rp;
    pinMode( leftPin, INPUT );
    pinMode( rightPin, INPUT );
    minPower = np;
    maxPower = xp;
    setThrottleRange( minPower, maxPower );
}

// just check if n is appropriate, then return the result
float BezierThrottleControl::factorial( int n ) {
    
    if( n < 0 ) {
        Serial2.println( "ERROR: n is less than 0" );
    }
    if( n > 32 ) {
        Serial2.println( "ERROR: n is greater than 32" );
    }
    
    return factorialLookup[n]; /* returns the value n! as a SUMORealing point number */
}

float BezierThrottleControl::Ni( int n, int i ) {
    
    float ni;
    float a1 = factorial( n );
    float a2 = factorial( i );
    float a3 = factorial( n - i );
    
    ni =  a1 / ( a2 * a3 );
    
    return ni;
}

// Calculate Bernstein basis
float BezierThrottleControl::Bernstein( int n, int i, float t ) {
    
    float basis;
    float ti; /* t^i */
    float tni; /* (1 - t)^i */
    
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

void BezierThrottleControl::Bezier2D( float b[], int bCount, int cpts, float p[] ) {
    
    int npts = bCount / 2;
    int icount, jcount;
    float step, t;
    
    // Calculate points on curve
    icount = 0;
    t = 0;
    step = (float)1.0 / ( cpts - 1 );
    for( int i1 = 0; i1 != cpts; i1++ ) {
        if( (1.0 - t) < 5e-6 ) {
            t = 1.0;
        }
        jcount = 0;
        p[icount] = 0.0;
        p[icount + 1] = 0.0;
        for ( int i = 0; i != npts; i++ ) {
            float basis = Bernstein( npts - 1, i, t );
            p[icount] += basis * b[jcount];
            p[icount + 1] += basis * b[jcount + 1];
            jcount = jcount + 2;
        }
        icount += 2;
        t += step;
    }
}

void BezierThrottleControl::setThrottleRange( uint8_t minPower, uint8_t maxPower ) {
    
    float inputs[8] = {
        1, minPower,
        (1024/4), maxPower,
        (1024/4)*3, minPower,
        1023, maxPower
    };
    float outputs[THROTTLE_MAP_COUNT * 2];
    
    Bezier2D( inputs, 8, THROTTLE_MAP_COUNT, outputs );
    
    for ( int x = 0; x < THROTTLE_MAP_COUNT * 2; x += 2 ) {
        throttleCurve[(int)(x/2)] = (long)outputs[x+1];
    }
    
}

void BezierThrottleControl::loop() {
    int readA = analogRead( A0 ) / 4;
    int readB = analogRead( A1 ) / 4;
    int cmdA = throttleCurve[readA];
    int cmdB = throttleCurve[readB];
    if( cmdA != lastCmdA || cmdB != lastCmdB ) {
        if( cmdA != lastCmdA ) {
            Serial3.write( cmdA );
            lastCmdA = cmdA;
        }
        if( cmdB != lastCmdB ) {
            Serial3.write( cmdB + 127 );
            lastCmdB = cmdB;
        }
        Serial2.print( "L: " + String( cmdA ) );
        Serial2.println( "  R: " + String( cmdB + 127 ) );
    }
    delay( 100 );
}



BezierThrottleControl throttleControl( 1, 127, A0, A1 );

// Add setup code 
void setup() {
    
    // debug output
    Serial2.begin( 115200 );
    Serial2.print( "Initializing system... " );

    Serial3.begin( 9600 );
    Serial3.write( 0 ); // full stop

    //throttleControl.setThrottleRange( 1, 127 ); // Advanced
    //throttleControl.setThrottleRange( 32, 96 ); // Regular
    //throttleControl.setThrottleRange( 48, 80 ); // Kiddie

    Serial2.println( "Ready!" );

}

// Add loop code
void loop() {
    throttleControl.loop();
}
