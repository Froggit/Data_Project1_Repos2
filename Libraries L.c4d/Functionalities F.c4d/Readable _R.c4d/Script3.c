/*-- Grundscript f�r alle lesbaren Dinge --*/

/*

bestes Beispiel w�ren Wallscripts, B�cher, Wegweiser, etc.

insgesamt soll alles einheitllich sein

Dialoge sind auch lesbar ;)

*/

#strict 2

/* Format f�r lesbare Dinge: eine eigene kleine Scriptsprache

Der Dialog selbst ist ein Array. Darin enthalten sind weitere Arrays, die nach folgendem Format aufgebaut sind:

[ int iIndex, array/int iParentIndex, string szMenuOption, string szText, int/string iObjectNr, array aMenuStyle, array aTextStyle , string/array aConditions, array aEvents ]

Dokumentation:

int iIndex:
	ein Index f�r den Dialog. Kinder-Knoten k�nnen so angeben, wer ihre Eltern sind. Eine indizierung �ber die Reihenfolge im Dialog-Array w�re auch m�glich gewesen, erschwert aber das dynamische zuweisen neuer Dialoge

array/int iParentIndex:
	Wird als Option an den Dialog iParentIndex angeh�ngt. Wird ein Array angegeben, dann wird es an alle Knoten im Array angeh�ngt

string szMenuOption:
	falls der Dialog als Option in einem Menu erscheint, dann wird dieser Text zur Auswahl angezeigt

string szText:
	das, was man sp�ter lesen kann - in den meisten F�llen reicht die MenuAuswahl, dann einfach nur "" oder 0 angeben

int/string iObjectNr:
	die Objektnummer des Sprechers.
	0 entspricht dem Objekt, in dem der Dialog ist
	-1 entspricht dem aufrufendem Objekt, d.h. in der Regel dem Spieler
	"String" entspricht dem Objekt GlobalN("String")

array aMenuStyle:
	[ id idIcon, int dwColor, int iStyle, int iExtra, XPar1, int iOverrideIndex, string szPortrait ]
	idIcon:
		ID des Menusymbols
	dwColor:
		Farbe, in welcher der Text angegeben werden soll
	iStyle:
		Stil, in welchem der Text dargestellt werden soll:
		0 - wird ausgeblendet, wenn die Voraussetzungen nicht erf�llt sind
		1 - wird rot eingeblendet, wenn die Voraussetzungen nicht erf�llt sind
	iExtra:
		siehe AddMenuItem
	XPar1:
		siehe AddMenuItem
	iOverrideIndex:
		springt zum angegebenen Dialogindex, anstatt die eigene Nachricht auszuw�hlen


array aTextStyle:
	[ bool fName, int dwColor, bool fInstant, bool fAsMessage, id idMenuDeco ]
	fName:
		soll der Name des Sprechers in dessen Farbe angezeigt werden? Standardm��ig ja
	dwColor:
		Farbe, in welcher der Rest des Texts angezeigt werden soll
	fInstant:
		soll der Text sofort angezeigt werden?
	szPortrait:
		gibt ein spezielles Portrait an
	fAsMessage:
		gibt die Nachricht als CustomMessage aus
	idMenuDeco:
		setzt eine neue MenuDecoration. �berschreibt auch die Definition aus GetDlgMsgDeco();

	wird anstatt eines Arrays -1 angegeben, dann wird kein neues Menu ge�ffnet

string/array aConditions:
	ist ein String aus einer Bedingung, oder ein Array aus mehreren Bedingungen
	die Bedingungen werden als String eingegeben. Dabei kann mit "pTarget" das ansprechende Objekt referenziert werden, und mit "pSpeaker" das angesprochene Objekt
	Beispiele:
	"GetName(pTarget)==\"Peter\"" ist true, wenn der Ansprechende "Peter" hei�t
	"FindContents(ROCK,pSpeaker)" ist true, wenn der Angesprochene einen Stein hat
	Die Bedingungen werden mit eval() ausgef�hrt, man sollte also mit objekterstellenden "Bedingungen" vorsichtig sein, kann aber auch ein bisschen tricksen ;)


string/array aEvents:
	siehe aConditions. Die Befehle werden ausgef�hrt, wenn der Dialog ausgew�hlt wird
	da f�r eval kein "if" erlaubt ist, muss man eine Funktion im Objekt aufrufen oder ein wenig tricksen:
	"!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));" l�scht einen Flint aus dem Objekt - falls kein Flint vorhanden wird aber das Objekt nicht gel�scht (der Parser erkennt || schon als wahr nachdem der Flint nicht vorhanden ist und f�hrt das L�schen nicht aus)


*/


local aDialogue, iStartDialogue;

// das Array manipulieren

public func SetDialogue( aDlg )
{
	// Direkteingabe
	if( GetType(aDlg) == C4V_Array )
		aDialogue = aDlg;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erh�lt!
	else if( aDlg )
		aDialogue = GameCall(Format("MsgDialogue%s", aDlg));
}

public func AddDialogue( aOption )
{
	if( GetType(aDialogue) == C4V_Array )
		PushBack( aOption, aDialogue );
}

public func GetUnusedDlgIndex()
{
	var aIndices = [];
	for( aOption in aDialogue )
		PushBack( aOption[0], aIndices );

	var i=0;
	for( i=0; i<= GetLength(aDialogue); i++)
		if(GetArrayItemPosition(i,aIndices)==-1)
			break;

	return i;
}


// Dialog aufbauen

public func StartDialogue( object pTarget )
{
	if( GetType(aDialogue) != C4V_Array ) return;

	// Begr��ung heraussuchen
	ProcessDialogue( pTarget, iStartDialogue );
}

protected func ProcessDialogue( object pTarget, int iDialogue )
{
	var aOption;
	for( aOption in aDialogue )
	{
		if( aOption[0] == iDialogue ) break;
	}

	// Daten aufbauen
	var iIndex = aOption[0];
	var iParentIndex = aOption[1];
	var szMenuOption = aOption[2];
	var szText = aOption[3];
	var iObjectNr = aOption[4]; //
	var aMenuStyle = aOption[5];
	var aTextStyle = aOption[6]; //
	var aConditions = aOption[7];
	var aEvents = aOption[8];

	// Events passieren lassen
	ProcessEvents( aEvents, pTarget );

	if( GetType(aTextStyle) == C4V_Int )
		if( aTextStyle == -1)
			return;

	// Farbe �berschreiben
	var dwClrOvrd = this->~GetDialogueColor();
	if( dwClrOvrd )
	{
		if( GetType(aTextStyle) == C4V_Array )
		{
			aTextStyle[1]=dwClrOvrd; // dwColor
		}
		else
		{
			aTextStyle = [];
			aTextStyle[0]=true; // fName
			aTextStyle[1]=dwClrOvrd; // dwColor
		}
	}

	// als Nachricht statt als Box
	var fAsMessage = false;
	
	if( GetType(aTextStyle) == C4V_Array ) fAsMessage = aTextStyle[4];

	// Sprecher raussuchen
	var pSpeaker = GetSpeaker();//this;
	if( iObjectNr == -1 ) pSpeaker = pTarget;
	if( GetType( iObjectNr ) == C4V_Int) if( iObjectNr > 0) pSpeaker = Object(iObjectNr);
	if( GetType( iObjectNr ) == C4V_String )
	{
		if( GlobalN( iObjectNr ) ) pSpeaker = GlobalN( iObjectNr );
		if( iObjectNr == "pTarget" ) pSpeaker = pTarget;
	}

	// Zufallsauswahl
	if( GetType(szText) == C4V_Array )
	{
		szText = szText[Random(GetLength(szText))];
	}

	MsgBox( pTarget, szText, pSpeaker , /* szPortrait*/ 0,  fAsMessage, true, aTextStyle );

	// Auswahl hinzuf�gen
	if(!fAsMessage)
	for( aOption in aDialogue )
	{
		var add = false;
		if( GetType(aOption[1]) == C4V_Array )
		{
			if( GetArrayItemPosition( iIndex, aOption[1] ) > -1 ) add = true;
		}
		else
		{
			if( aOption[1] == iIndex ) add = true;
			if( aOption[1] == -1 && aOption[2] ) add = true; // nur, wenn er eine Menu-Auswahl hat!
		}

		if( add ) ProcessDialogueOption( pTarget, aOption[0] );
	}
}

protected func ProcessDialogueOption( object pTarget, iDialogue )
{
	var aOption;
	for( aOption in aDialogue )
	{
		if( aOption[0] == iDialogue ) break;
	}

	// Daten aufbauen
	var iIndex = aOption[0];
	var iParentIndex = aOption[1];
	var szMenuOption = aOption[2];
	var szText = aOption[3];
	var iObjectNr = aOption[4];
	var aMenuStyle = aOption[5];
	var aTextStyle = aOption[6];
	var aConditions = aOption[7];
	var aEvents = aOption[8];

	// vorerst gibt es noch keine Conditions
	var fAdd = false;

	if( CheckConditions( aConditions, pTarget )) fAdd = true;

	if( szMenuOption == 0 || szMenuOption == "" )
	{
		if( szText )
		{
			// hier fehlt noch die Verarbeitung von aTextStyle
			MsgBoxAddText(pTarget, szText);
		}
	}
	else
	{
		var idIcon = NONE;
		var extra, xPar;
		var iStyle = 0;
		var dwCol, iIndexOvr = iIndex;

		if( GetType(aMenuStyle) == C4V_Array )
		{
			idIcon = aMenuStyle[0];
			dwCol = aMenuStyle[1];
			iStyle = aMenuStyle[2];
			extra = aMenuStyle[3];
			xPar = aMenuStyle[4];
			if( aMenuStyle[5] != 0 ) iIndexOvr = aMenuStyle[5]-1;
		}
		if( GetType(aMenuStyle) == C4V_C4ID )
			idIcon = aMenuStyle;


		// Farbe �berschreiben
		var dwClrOvrd = this->~GetDialogueOptionColor();
		if( dwClrOvrd ) dwCol = dwClrOvrd;
		if( fAdd == false ) dwCol = RGB(255,0,0);
		if( dwCol ) szMenuOption=ColorizeString(szMenuOption,dwCol);

		var szCommand=Format("ProcessDialogue(Object(%d),%d)",ObjectNumber(pTarget),iIndexOvr );
		if( fAdd == false && iStyle == 1) szCommand = "eval(\"true\")";//"";

		if( fAdd || iStyle == 1 )
			MsgBoxAddOption( pTarget, idIcon, szMenuOption, szCommand, 0, extra, xPar);
	}

}

protected func CheckConditions( aConditions, object pTarget)
{
	// keine Bedingungen sind immer erf�llt
	if(!aConditions) return true;
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) return true;

	var fFulfilled = true; // im Zweifel f�r den Angeklagten
	var aCond;

	if( GetType( aConditions ) == C4V_String )
		fFulfilled = CheckCondition( aConditions, pTarget );
	else
	{
			for( aCond in aConditions )
				// die erste nicht-erf�llte Bedingung macht ihn schuldig ;)
				if(!CheckCondition( aCond, pTarget ))
				{
					fFulfilled = false;
					break;
				}
	}


	return fFulfilled;
}

protected func ProcessEvents( aEvents, object pTarget)
{
	// keine Bedingungen sind immer erf�llt
	if(!aEvents) return true;
	if( GetType(aEvents) == C4V_Array ) if(GetLength(aEvents) < 1) return true;

	var aEv;

	if( GetType( aEvents ) == C4V_String )
		CheckCondition( aEvents, pTarget );
	else
	{
		for( aEv in aEvents )
			CheckCondition( aEv, pTarget );
	}

}

protected func CheckCondition( aCondition, pTarget )
{
	// keine Bedingungen sind immer erf�llt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	szEval = ReplaceAll(szEval,"pTarget",Format("Object(%d)",ObjectNumber(pTarget)));
	szEval = ReplaceAll(szEval,"pSpeaker", Format("Object(%d)",ObjectNumber(GetSpeaker())));

	DebugLog("Evaluating \"%s\"",szEval);

	var result = eval( szEval );
	DebugLog("Evaluation: %v", result);
	return result; //eval( szEval );
}

public func GetSpeaker(){ return this; }