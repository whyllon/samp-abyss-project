//3DMenu. Author: SDraw
/*#include <a_samp>
#include <YSI\y_hooks>
#include <streamer>
#include <YSI\y_iterate>*/
/* Fake natives. Thanks to TheArcher.
native Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid);
native SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,selectcolor,unselectcolor,textalignment);
native Select3DMenu(playerid,MenuID);
native CancelSelect3DMenu(playerid);
native Destroy3DMenu(MenuID);
*/

#define MAX_3DMENUS (MAX_PLAYERS+10)
#define MAX_BOXES (16)

new SelectedMenu[MAX_PLAYERS];
new SelectedBox[MAX_PLAYERS];
enum MenuParams { Float:Rotation,Boxes,bool:IsExist,
	Objects[MAX_BOXES],Float:OrigPosX[MAX_BOXES],Float:OrigPosY[MAX_BOXES],Float:OrigPosZ[MAX_BOXES],
	Float:AddingX,Float:AddingY,SelectColor[MAX_BOXES],UnselectColor[MAX_BOXES] };
new MenuInfo[MAX_3DMENUS][MenuParams];

//Callbacks
forward OnPlayerSelect3DMenuBox(playerid,MenuID,selected);
forward OnPlayerChange3DMenuBox(playerid,MenuID,boxid);

stock Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid)
{
	if(boxes > MAX_BOXES || boxes <= 0) return -1;
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    if(!MenuInfo[i][IsExist])
	    {
	        new Float:NextLineX,Float:NextLineY;
	        new lineindx,binc;
	        MenuInfo[i][Rotation] = rotation;
			MenuInfo[i][Boxes] = boxes;
			MenuInfo[i][AddingX] = 0.25*floatsin(rotation,degrees);
			MenuInfo[i][AddingY] = -floatcos(rotation,degrees)*0.25;
   			NextLineX = floatcos(rotation,degrees)+0.05*floatcos(rotation,degrees);
   			NextLineY = floatsin(rotation,degrees)+0.05*floatsin(rotation,degrees);
	        for(new b = 0; b < boxes; b++)
			{
			    if(b%4 == 0 && b != 0) lineindx++,binc+=4;
			    MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x+NextLineX*lineindx,y+NextLineY*lineindx,z+1.65-0.55*(b-binc),0,0,rotation,-1,-1,playerid,100.0);
	            GetDynamicObjectPos(MenuInfo[i][Objects][b],MenuInfo[i][OrigPosX][b],MenuInfo[i][OrigPosY][b],MenuInfo[i][OrigPosZ][b]);
			}
			MenuInfo[i][IsExist] = true;
			return i;
		}
	}
	return -1;
}

stock SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,selectcolor,unselectcolor,textalignment)
{
	if(!MenuInfo[MenuID][IsExist]) return -1;
	if(box == MenuInfo[MenuID][Boxes] || box < 0) return -1;
	if(MenuInfo[MenuID][Objects][box] == INVALID_OBJECT_ID) return -1;
	MenuInfo[MenuID][SelectColor][box] = selectcolor;
	MenuInfo[MenuID][UnselectColor][box] = unselectcolor;
	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][box],0,text,materialsize,fontface,fontsize,bold,fontcolor,unselectcolor,textalignment);
	return 1;
}

stock Select3DMenu(playerid,MenuID)
{
	if(!MenuInfo[MenuID][IsExist]) return -1;
	if(SelectedMenu[playerid] != -1) CancelSelect3DMenu(playerid);
	SelectedBox[playerid] = 0;
	new text[128],materialsize,fontface[128],fontsize,bold,fontcolor,backcolor,textalignment;
	GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][0],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
	SelectedMenu[playerid] = MenuID;
 	MoveDynamicObject(MenuInfo[MenuID][Objects][0],MenuInfo[MenuID][OrigPosX][0]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][0]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][0],1.0);
 	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][0],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][0],textalignment);
	return 1;
}

#if defined FILTERSCRIPT
hook OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
 		SelectedMenu[i] = -1;
		SelectedBox[i] = -1;
	}
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
     	MenuInfo[i][Boxes] = 0;
	 	MenuInfo[i][IsExist] = false;
	 	MenuInfo[i][AddingX] = 0.0;
 		MenuInfo[i][AddingY] = 0.0;
	}
}
hook OnFilterScriptExit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
}
#else
hook OnGameModeInit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
	    for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
	    MenuInfo[i][Boxes] = 0;
	    MenuInfo[i][IsExist] = false;
	    MenuInfo[i][AddingX] = 0.0;
 		MenuInfo[i][AddingY] = 0.0;
	}
}
hook OnGameModeExit()
{
	for(new i = 0; i < MAX_3DMENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
}
#endif
hook OnPlayerConnect(playerid)
{
    SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
}

hook OnPlayerKeyStateChange(playerid,newkeys,oldkeys)
{
	if(SelectedMenu[playerid] != -1)
	{
		new MenuID = SelectedMenu[playerid];
	    if(newkeys == KEY_NO)
	    {
	        new text[128],materialsize,fontface[128],fontsize,bold,fontcolor,backcolor,textalignment;
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			SelectedBox[playerid]++;
			if(SelectedBox[playerid] == MenuInfo[MenuID][Boxes]) SelectedBox[playerid] = 0;
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][SelectedBox[playerid]],textalignment);
		}
		if(newkeys == KEY_YES)
	    {
	        new text[128],materialsize,fontface[128],fontsize,bold,fontcolor,backcolor,textalignment;
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
	        MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			SelectedBox[playerid]--;
			if(SelectedBox[playerid] < 0) SelectedBox[playerid] = MenuInfo[MenuID][Boxes]-1;
			MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]]+MenuInfo[MenuID][AddingX],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]]+MenuInfo[MenuID][AddingY],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
			GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
			SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][SelectColor][SelectedBox[playerid]],textalignment);
		}
		if(newkeys == KEY_NO || newkeys == KEY_YES) CallLocalFunction("OnPlayerChange3DMenuBox","idd",playerid,MenuID,SelectedBox[playerid]);
		if(newkeys == KEY_SPRINT) CallLocalFunction("OnPlayerSelect3DMenuBox","idd",playerid,MenuID,SelectedBox[playerid]);
	}
}

stock CancelSelect3DMenu(playerid)
{
	if(SelectedMenu[playerid] == -1) return -1;
	new MenuID = SelectedMenu[playerid];
	new text[128],materialsize,fontface[128],fontsize,bold,fontcolor,backcolor,textalignment;
	GetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][SelectedBox[playerid]],0,text,materialsize,fontface,fontsize,bold,fontcolor,MenuInfo[MenuID][UnselectColor][SelectedBox[playerid]],textalignment);
	MoveDynamicObject(MenuInfo[MenuID][Objects][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosX][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosY][SelectedBox[playerid]],MenuInfo[MenuID][OrigPosZ][SelectedBox[playerid]],1.0);
	SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
	return 1;
}

stock Destroy3DMenu(MenuID)
{
    if(!MenuInfo[MenuID][IsExist]) return -1;
    foreach(new i : Player) if(SelectedMenu[i] == MenuID) CancelSelect3DMenu(i);
    for(new i = 0; i < MenuInfo[MenuID][Boxes]; i++)
    {
		DestroyDynamicObject(MenuInfo[MenuID][Objects][i]);
		MenuInfo[MenuID][Objects][i] = INVALID_OBJECT_ID;
	}
 	MenuInfo[MenuID][Boxes] = 0;
 	MenuInfo[MenuID][IsExist] = false;
 	MenuInfo[MenuID][AddingX] = 0.0;
 	MenuInfo[MenuID][AddingY] = 0.0;
	return 1;
}
