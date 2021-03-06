/*-- Talker --*/

#strict 2
#include LF_R // ist "lesbar" ;)

local talkto;

public func IgnoreInventory(){ return true; }

public func MenuQueryCancel(){ return 1; }

global func AttachDialogue( object pTarget, vDlg )
{
	var talker = CreateObject(TALK,0,0,-1);
	talker->~Set( pTarget, vDlg );
}

public func Set(object target, vDlg)
{
  SetAction("Attach",target);
  SetName(GetName(target));
  talkto = target;
  SetDialogue( vDlg );
  SetVisibility(VIS_None);
}

public func GetMsgPortrait()
{
  return(Format("Portrait:%i::%x::%s", GetID(talkto), GetColorDw(talkto), "1"));
}

public func GetDlgMsgColor(){ return GetColorDw(talkto); }

public func ActivateEntrance(object pBy)
{
	SetCommand(pBy, "None");
	if(!(GetOCF(pBy) & OCF_CrewMember)) return;
	
	StartDialogue( pBy );
}

public func GetSpeaker(){ return talkto; }
