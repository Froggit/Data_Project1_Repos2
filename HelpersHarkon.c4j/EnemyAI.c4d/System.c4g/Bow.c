/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto BOW1

/* Pfeil abfeuern */
public func FireSpeed(vx, vy, iAngle)
{
  var pClonk = Contained();
  // Kein Tr�ger?
  if(!pClonk) return 0;
  // Im Geb�ude?
  if (pClonk->Contained()) { pClonk->~StopAiming(); return 0; }
  // Unvollst�ndig?
  if (~GetOCF() & OCF_Fullcon) return 0;
  // Noch beim Nachladen
  if (GetAction() == "Reload") return 0;
  // Pfeil vom Tr�ger holen
  var pArrow = Contained()->~GetArrow();
  if (!pArrow) return;
  // Beim Tr�ger ggf. Bogen wieder ausw�hlen
  ShiftContents(Contained(), 0, BOW1);
  // Austrittsgeschwindigkeit ermitteln
  var iOutX, iOutY, iOutR, iOutXDir, iOutYDir, iOutRDir;
  if (!pClonk->~GetBowOut(this(), iAngle, true, iOutX, iOutY, iOutR, iOutXDir, iOutYDir, iOutRDir))
    GetBowOutAngle(Contained(), iAngle, true, iOutX, iOutY, iOutR, iOutXDir, iOutYDir, iOutRDir);
  // Besitzer des Projektils setzen
  pArrow->SetOwner(Contained()->GetOwner());
    // Pfeil abfeuern
  Exit(pArrow, 0,0,0, 0,0,0 );
	pArrow->SetPosition(GetX(), GetY());
  pArrow->SetXDir(vx); pArrow->SetYDir(vy);
  pArrow->~Launch();
    // Sound
  Sound("Bow");
  // Nachladen
  SetAction("Reload");
  Contained()->~LoadBow();
  return 1;
}
