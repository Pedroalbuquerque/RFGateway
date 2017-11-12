#ifdef DEBUG_PORT
    char buf[100];

    #if defined( ARDUINO_AVR_MOTEINO) || defined(ARDUINO_AVR_MOTEINOMEGA)
      #define DEBUG_MSG(...) sprintf( buf,__VA_ARGS__ );DEBUG_PORT.print(buf);
    #else
      #define DEBUG_MSG(...) DEBUG_PORT.printf( __VA_ARGS__ );
    #endif
#else
    #define DEBUG_MSG(...)
#endif
