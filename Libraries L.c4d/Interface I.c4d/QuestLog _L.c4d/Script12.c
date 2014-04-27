/*-- Buch --*/

#strict 2
#include LF_R // ist lesbar, oh Wunder :)

local aQuestIndex;
local aQuestOpen;
local aQuestDone;
local aQuestFail;
local aQuestDesc;
local aQuestRead;

local aDialogue;
local aQuestOpenPageNrs;
local aQuestFailPageNrs;
local aQuestDonePageNrs;

// integer limit = 2147483647
static const g_iQuestLogMax = 10000;
static const g_iQuestLogEntriesPerPage = 5;
static const g_iQuestLogPageConstant = 12001;//(g_iQuestLogEntriesPerPage + 1) * g_iQuestLogMax / g_iQuestLogEntriesPerPage + 1; // x quests + x/EntriesPerPage pages , x = 10000;
static const g_iQuestFailed = 12002; //g_iQuestLogPageConstant + 1;
static const g_iQuestDone =   24004;//2*g_iQuestLogPageConstant + 2;
static const g_iQuestLogQuestIndex = 36006;//3*g_iQuestLogPageConstant + 3;

/*
public func QuestColorText(){ return RGB(255,255,0);}
public func QuestColorNew(){ return RGB(255,0,0);}
public func QuestColorOld(){ return RGB(0,255,0);}
public func QuestColorTitle(){ return RGB(0,0,255); }
public func QuestColorMenu(){ return RGB(0,255,255); }
public func QuestIconDone(){ return TALK;}
public func QuestIconFail(){ return MCMX;}
*/
public func QuestColorText(){ return RGB(255,255,255);}
public func QuestColorNew(){ return RGB(255,0,0);}
public func QuestColorOld(){ return RGB(200,200,200);}
public func QuestColorTitle(){ return RGB(255,255,255); }
public func QuestColorMenu(){ return RGB(255,255,255); }
public func QuestIconDone(){ return RSR1;}//TALK;}
public func QuestIconFail(){ return RSR2;}//MCMX;}

public func QuestLogArray(){ return [false,QuestColorMenu(),0,0,false,-1,"Journal"]; }

public func AddQuest(szName, bool bNoSound)
{
	if(!bNoSound) Sound("Ding",1,0,0,GetOwner()+1);
	//PushFront(szName,aQuestOpen);
	//PushBack(szName,aQuestIndex);
	//PushBack(0,aQuestDesc);
	PushBack(szName,aQuestOpen);
	PushFront(szName,aQuestIndex);
	PushFront(0,aQuestDesc);
	PushFront(0,aQuestRead);
}

public func UpdateQuest(string szName, bool bNoSound)
{
	Log("UpdateQuest called");
	//if(!bNoSound) Sound("Ding",1,0,0,GetOwner()+1);
	//DropArrayItem(szName,aQuestOpen);
	//PushFront(szName,aQuestOpen);

	//PushBack(szName,aQuestOpen);

}

public func FinishQuest(szName)
{
	Log("Finished Quest");
	Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestDone);
	DropArrayItem(szName,aQuestOpen);
}

public func FailQuest(szName)
{
	Log("Failed Quest");
	PushFront(szName,aQuestFail);
	DropArrayItem(szName,aQuestOpen);
}

protected func Initialize()
{
	//SetColorDw(HSL(Random(256), 255, Random(128)+128));
	aQuestOpen = aQuestDone = aQuestFail = aQuestDesc = aQuestIndex = aQuestRead = [];

}

public func Activate(object pPlayer)
{
	[$CtrlRead$]

	SetOwner(GetOwner(pPlayer));
	ConstructDialogue(pPlayer);
	StartDialogue( pPlayer );
	return 1;
}

// Baut die Dialogfunktion des Buches auf
public func ConstructDialogue( pPlayer )
{
	// bisherigen Dialog l�schen
	aDialogue = [];

	local iIndex;
	iIndex = 1;
	var iMenuPage = 1;
	var iEntriesPerPage = 5;
	var iMenuPages = Max((GetLength(aQuestOpen)+iEntriesPerPage-1)/iEntriesPerPage,1);
	var iMenuPagesFail = Max((GetLength(aQuestFail)+iEntriesPerPage-1)/iEntriesPerPage,1);
	var iMenuPagesDone = Max((GetLength(aQuestDone)+iEntriesPerPage-1)/iEntriesPerPage,1);
	var iMenuPageParent = 0;
	var iMenuPageParentOld = 0;
	var iMenuPageParentOld2 = 0;

	var aLayerButtons = [];
	var aPagesIndex = [0];

	// alle offenen Quests anzeigen
	var dlgOpenQuest = [0,-1,0,ColorizeString(Format("Offene Quests (%d/%d)",iMenuPage,iMenuPages),QuestColorMenu()),0,SCRG,QuestLogArray()];
	// die abgeschlossenen Quests
	var dlgDoneQuest = [g_iQuestDone,aPagesIndex,"Abgeschlossene Quests",ColorizeString(Format("Abgeschlossene Quests (%d/%d)",iMenuPage,iMenuPagesDone),QuestColorMenu()),0,[QuestIconDone(),QuestColorMenu()],QuestLogArray()];
	// die gescheiterten Quests
	var dlgFailQuest = [ g_iQuestFailed, aPagesIndex,"Gescheiterte Quests",ColorizeString(Format("Gescheiterte Quests (%d/%d)",iMenuPage,iMenuPagesFail),QuestColorMenu()),0,[QuestIconFail(),QuestColorMenu()],QuestLogArray()];


	// Questindices bauen
	var entriesperpage = 5;
	var aDlgQuestOpen, aDlgQuestDone, aDlgQuestFail, aDlgQuestPages;
	aDlgQuestOpen = aDlgQuestDone = aDlgQuestFail = aDlgQuestPages = [];
	var aDlgDesc, aDlgDescPages;
	aDlgDesc = aDlgDescPages = [];

	BuildQuestPages( 0, pPlayer, "Offene Quests", entriesperpage, aQuestOpen, aDlgQuestOpen, aDlgQuestPages, aDlgDesc, aDlgDescPages);
	BuildQuestPages( g_iQuestDone, pPlayer, "Abgeschlossene Quests", entriesperpage, aQuestDone, aDlgQuestDone, aDlgQuestPages, aDlgDesc, aDlgDescPages);
	BuildQuestPages( g_iQuestFailed, pPlayer, "Gescheiterte Quests", entriesperpage, aQuestFail, aDlgQuestFail, aDlgQuestPages, aDlgDesc, aDlgDescPages);

	// Dialoge in der richtigen Reihenfolge anh�ngen:
	PushBack(dlgOpenQuest,aDialogue);

	for(var dlg in aDlgQuestOpen)
		PushBack(dlg,aDialogue);

	PushBack(dlgDoneQuest,aDialogue);
	for(var dlg in aDlgQuestDone)
		PushBack(dlg,aDialogue);

	PushBack(dlgFailQuest,aDialogue);
	for(var dlg in aDlgQuestFail)
		PushBack(dlg,aDialogue);


	for(var dlg in aDlgQuestPages)
		PushBack(dlg,aDialogue);

	for(var dlg in aDlgDesc)
	{
		//Log("Pushing back %v", dlg);
		PushBack(dlg,aDialogue);
	}

	for(var dlg in aDlgDescPages)
	{
		//Log("Pushing back %v", dlg);
		PushBack(dlg,aDialogue);
	}

	// letzter Schritt
	// an alle ein "Hauptmenu" anh�ngen
	//var aList = [];
	//for(var i = 0; i < iIndex; i++) aList[i]=i+1;
	//FilterArray(aList,aPagesIndex,true);

	//iIndex++;

	var aList = [g_iQuestDone, g_iQuestFailed];
	PushBack([-5,aList,"Hauptmenu","", 0, [LMM2,QuestColorMenu(),0,2,0,0], -1, "true", ["StopDialogue(pTarget)","Activate(pTarget)"] ],aDialogue);
}

protected func BuildQuestPages(int iPageIndex, object pPlayer, string szPageName, int iEntriesPerPage, array aQuestQueue, array &aDlgQuest, array &aDlgPages, array &aDlgDesc, array &aDlgDescPages)
{
	var iMenuPages = Max((GetLength(aQuestQueue)+iEntriesPerPage-1)/iEntriesPerPage,1);

	Log("strange: length = %d, mpag = %d", GetLength(aQuestQueue), iMenuPages);
	for(var i = 1; i <= iMenuPages; i++)
	{
		Log("Adding quest pages: %d / %d", i, iMenuPages);
		if(i < iMenuPages )
		{
			var iPagesIndexForw = iPageIndex+i-1;
			//Log("* Added forward button: %d %d ",iPageIndex+i,iPagesIndexForw);
			PushBack([iPageIndex+i,iPagesIndexForw,Format("Seite %d vorw�rts, parents: %d",i+1,iPagesIndexForw),ColorizeString(Format("%s (%d/%d)", szPageName,i+1,iMenuPages),QuestColorMenu()),0,[LMM2,QuestColorMenu(),0,2,4],QuestLogArray()],aDlgPages);
			Log("forward %s (%d/%d)", szPageName,i+1,iMenuPages);
		}
		if(i > 1)
		{
			var iPagesIndexBack = iPageIndex+i-1;
			//Log("* Added back button: %d %d ",iPageIndex+iMenuPages+i-2,iPagesIndexBack);
			PushBack([iPageIndex+iMenuPages+i-2,iPagesIndexBack,Format("Seite %d r�ckw�rts, parents: %d",i-1,iPagesIndexBack),ColorizeString(Format("%s (%d/%d) Back", szPageName,i-1,iMenuPages),QuestColorMenu()),0,[LMM2,QuestColorMenu(),0,2,0,iPagesIndexBack], [false,0,0,0,false,-1,"Journal"] ],aDlgPages);
			Log("forward %s (%d/%d)", szPageName,i+1,iMenuPages);
		}

	}

	for(var i=0; i<GetLength(aQuestQueue); i++)
	{
		var dialogueID = iPageIndex + 2*iMenuPages +i + 1;
		var parentID = iPageIndex + i/iEntriesPerPage;

		//Log("i is %d, dialogueID %d, %d, %d, %d",i, dialogueID, iPageIndex + 2*iMenuPages, iPageIndex + 2*iMenuPages + 1, iPageIndex + 2*iMenuPages + i +1);

		// checks if quest in the queue
		var quest = aQuestQueue[i];
		var qindex = GetArrayItemPosition(quest,aQuestIndex);
		var qindex2 = g_iQuestLogQuestIndex + qindex*100000;
		var log = GetQuestLog( quest, pPlayer );
		if( GetType(log) != C4V_Array ) continue;

		var szName = log[0][1];
		var iLines, bNew, iSigns, iIndexAdd;

		// Dialogoption zusammenbauen
		var iOldLen = aQuestDesc[qindex];
		if(GetLength(log) != iOldLen) aQuestRead[qindex]=0;
		aQuestDesc[qindex]=GetLength(log);


		// der Eintrag mit dem Namen auf der Hauptseite
		var dwClr = QuestColorOld();
		if(!aQuestRead[qindex]) dwClr = QuestColorNew();

		var aMenuStyle = [ NONE, dwClr ];

		var aTextStyle = [ false, QuestColorMenu(), true, 0, false, -1, "Journal" ];

		var aDescQueue = GetQuestDescArray(log);
		var iEntriesPerPage2 = 2;
		var iMenuPages2 = Max((GetLength(aDescQueue)+iEntriesPerPage2-1)/iEntriesPerPage2,1);

		var szNameExpanded;
		if(iMenuPages2 <= 1)
		{
			szNameExpanded = szName;
		}
		else
		{
			szNameExpanded = Format("%s (1/%d)", szName, iMenuPages);
		}

		var aDlg = [ dialogueID, parentID, szName, ColorizeString(szNameExpanded,QuestColorTitle()), 0, aMenuStyle, aTextStyle , "true", Format("aQuestRead[%d]=1",qindex) ];
		PushBack(aDlg,aDlgQuest);

		BuildQuestDescs(parentID, dialogueID, qindex2, pPlayer, szName, iEntriesPerPage2, aDescQueue, aDlgDesc, aDlgDescPages);
	}
}

protected func BuildQuestDescs(int iQuestMenuIndex, int iQuestPageIndex, int iPageIndex, object pPlayer, string szPageName, int iEntriesPerPage, array aDescQueue, array &aDlgDesc, array &aDlgPages)
{
	var iMenuPages = Max((GetLength(aDescQueue)+iEntriesPerPage-1)/iEntriesPerPage,1);

	var aBackMenuList = [iQuestPageIndex];
	for(var i = 1; i <= iMenuPages; i++)
	{
		//Log("* Adding description pages: %d / %d", i, iMenuPages);
		if(i < iMenuPages )
		{
			var iPagesIndexForw;// = iPageIndex+i-1;

			if(i==1)
			{
				iPagesIndexForw = iQuestPageIndex;
			}
			else
			{
				iPagesIndexForw = iPageIndex+i-1;
			}
			//Log("* * Added forward button: %d %d ",iPageIndex+i,iPagesIndexForw);
			PushBack([iPageIndex+i,iPagesIndexForw,Format("Seite %d",i+1,iPagesIndexForw),ColorizeString(Format("%s (%d/%d)", szPageName,i+1,iMenuPages),QuestColorMenu()),0,[LMM2,QuestColorMenu(),0,2,4],QuestLogArray()],aDlgPages);
			PushBack(iPageIndex+i,aBackMenuList);
		}
		if(i > 1)
		{
			var iPagesIndexBack; // = iPageIndex+i-1;
			var iPagesIndexJumpTo;

			iPagesIndexBack = iPagesIndexJumpTo = iPageIndex+i-1;

			if(i==2)
			{
				iPagesIndexJumpTo = iQuestPageIndex+1;
			}

			//Log("* * Added back button: %d %d ",iPageIndex+iMenuPages+i-2,iPagesIndexBack);
			PushBack([iPageIndex+iMenuPages+i-2,iPagesIndexBack,Format("Seite %d",i-1,iPagesIndexBack),ColorizeString(Format("%s (%d/%d) Back", szPageName,i-1,iMenuPages),QuestColorMenu()),0,[LMM2,QuestColorMenu(),0,2,0,iPagesIndexJumpTo], [false,0,0,0,false,-1,"Journal"] ],aDlgPages);
		}
	}

	//PushBack([iPageIndex + 2*iMenuPages +i-1, aBackMenuList, "Zur�ck zum Questindex", "bla" ,0,[LMM2,QuestColorMenu(),0,2,0,iQuestMenuIndex], [false,0,0,0,false,-1,"Journal"]],aDlgPages);
	PushBack([iPageIndex + 2*iMenuPages -1, aBackMenuList, Format("Hauptmenu",iQuestMenuIndex), "",0,[LMM2,QuestColorMenu(),0,2,0,iQuestMenuIndex+1], [false,0,0,0,false,-1,"Journal"]],aDlgPages);

	for(var i=0; i<GetLength(aDescQueue); i++)
	{
		var dialogueID = iPageIndex + 2*iMenuPages +i + 2;
		var parentID = i/iEntriesPerPage;

		if(!parentID)
		{
			parentID = iQuestPageIndex;
		}
		else
		{
			parentID += iPageIndex;
		}

		var aMenuStyle = [ NONE ];
		var aTextStyle = [ false, QuestColorMenu(), true, 0, false, -1, "Journal" ];

		var aDlg = [ dialogueID, parentID, "", aDescQueue[i], 0, aMenuStyle, aTextStyle ];
		PushBack(aDlg,aDlgDesc);
	}
}

public func GetQuestDescArray( log )
{
	var aDescContent = [];
	var iSigns, iLines;
	var szName = log[0][1];


	// der Eintrag mit dem Namen auf der Hauptseite
	//////////////////////////////////////////////////////////////////////////////////
	// au�erdem eine Questbeschreibung, die je 2 max. Eintr�ge als Kind von iParent einnimmt
	// und dann einen "-> zur�ckbl�ttern"-Eintrag hat, sowie einen "<- zur�ck"

	// Questbeschreibung zusammenbauen
	var szDesc = "", szLastspeaker = "";
	for( var j=GetLength(log)-1; j >= 0; j-- )
	{
		//Log("j %d / %d",j,GetLength(log)-1);
		var szSpeaker = log[j][0], szText = log[j][1];
		var szNewDesc = "";

			if( szDesc == "")
			{
				szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
			}
			else
			{
				szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
			}

		szNewDesc = ReplaceAll(szNewDesc,"(null)|(null)","");

		szLastspeaker = szSpeaker;

		iSigns = GetLength(szNewDesc)+GetLength(szDesc);
		iLines = GetLetterCount("|",szNewDesc) + GetLetterCount("|",szDesc) + iSigns/30;

		// zu viel? einen leer-Eintrag machen und den iIndexAdd hochz�hlen
		if(( iSigns > 240 || iLines > 8 ) && !( GetLetterCount("|",szDesc) > 8))
		{
			PushBack( szDesc ,aDescContent);
			szDesc=szNewDesc;
		}
		// passt!
		else
		{
			if( szDesc == "" )
				szDesc = szNewDesc;
			else
				szDesc = Format("%s|%s",szDesc,szNewDesc);

			if( j==0 )
			{
				PushBack( szDesc,aDescContent);
			}

		}
	}

	//Log("*** Final Quest Description for: %s", szName);
	for( var entry in aDescContent)
	{
		Log("    %s", entry);
	}

	return aDescContent;
}

