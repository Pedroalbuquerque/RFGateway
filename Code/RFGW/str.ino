int strMid(char * dstStr, char* srcStr, uint8_t start, uint8_t numChars){
  int i;
  for ( i = 0; i<numChars; i++){
    dstStr[i] = srcStr[start+i];
  }
  dstStr[i] = (char)NULL ;
  //DEBUG_MSG("\n\n[strMid]%s\n",dstStr);
  return i;
}
