/*-- Grundscript f�r Ausr�stungsoverlays --*/

#strict 2

protected func Initialize()
{
	SetClrModulation(RGB(255,255,255));
	_inherited(...);
}

public func GetOverlayGraphics(){ return ""; }
public func GetOverlayColor(){ return GetClrModulation(); }
