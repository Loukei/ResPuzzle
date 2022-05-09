// ResPuzzle.cpp : 定義主控台應用程式的進入點。
//

// Include librarys
#include "stdafx.h"
//#include "ResPuzzleModel.h"
//#include "ResPuzzleView.h"
#include "ResPuzzleController.h"
#include <iostream>
#include <vector>
#include <string>
using std::vector;

#define MAIN
#include "globals.h"

int main(int argc, char* argv[])
{
	CVoxelSpace vs;
	//before
	vs.ReadJSON("input/al-2.json");
	//GLVoxelModel* ptr_originalmodel = CreateModelByOriginal(&vs);

	//after
	vs.FillInInsideVoxel();
	GLVoxelModel* ptr_refillmodel = CreateModelByOriginal(&vs);

	remain_data rdata;
	InitializeRemainData(&vs, &rdata);

	PuzzlePiece P1 = CreatePuzzleForP1(&vs, &rdata);
	//std::cout << "dectet p1 is success:" << std::boolalpha << IsPuzzleSuccess(&vs, &rdata, &P1) << std::endl;
	GLVoxelModel* ptr_rest = IsPuzzleSuccess(&vs, &rdata, &P1);
	GLVoxelModel* ptr_P2 = CreatePuzzleForP2(&vs, &rdata, &P1);

	GLVoxelModel* ptr_p1 = CreateModelByPuzzle(&vs, P1);
	// CreateModelByOriginal(&vs);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	mainWndID = Create_glutMainWnd("main window", ptr_refillmodel);

	subWndID = Create_glutSubWnd("P1", ptr_P2);

	//Create_glutAnimateWnd("p1 expand range", &ani);

	//	Send to GLUT main loops
	glutMainLoop();

	system("PAUSE");
	return 0;
}