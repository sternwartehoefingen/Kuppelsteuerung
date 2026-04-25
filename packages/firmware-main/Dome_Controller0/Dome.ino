void Dome_showAzimuth(bool details)
{
  //    Serial.print(" az:");
  Serial.print("A");
  SerialPrint_int4((encoderPos * 10 * ((720UL * 32768 / MOUNTPOSMAX + 1) / 2)) / 32768);
  Serial.print(" ");
}
