#include "model2.h"

bool Model2::loadOBJ(const char * path, float *&vert, float *&tex, float *&normalrvert, int *numver, int *numtex, int *numnormv)
{

	FILE * file;
	errno_t err;
	err = fopen_s(&file, path, "r");
	if (err != NULL) {
		cout << "Impossible to open the file !\n";
		return false;
	}
	while (mod != 0) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0) {

			fscanf_s(file, "%f %f %f\n", &tab1[num1++], &tab2[num2++], &tab3[num3++]);

		}
		else if (strcmp(lineHeader, "vt") == 0) {
			fscanf_s(file, "%f %f\n", &tabvt1[numvt1++], &tabvt2[numvt2++]);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			fscanf_s(file, "%f %f %f\n", &tabvn1[numvn1++], &tabvn22[numvn2++], &tabvn33[numvn3++]);
		}
		else if (strcmp(lineHeader, "f") == 0) {

			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &fv[nfv++], &ft[nft++], &fn[nfn++], &fv[nfv++], &ft[nft++], &fn[nfn++], &fv[nfv++], &ft[nft++], &fn[nfn++]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}

		}
	}
	int numVertices2 = num1 + num2 + num3;
	int numTextures = numvt1 + numvt2;


	float *vertices2/*vert*/ = new float[numVertices2];
	float *textures2 /*tex*/ = new float[numTextures];

	int k = 0;

	float **vertice = new float*[num1];
	for (int i = 0; i < num1; i++)
		vertice[i] = new float[3];

	for (int i = 0; i < num1; i++)
	{
		vertice[i][k++] = tab1[i];
		vertice[i][k++] = tab2[i];
		vertice[i][k++] = tab3[i];
		k = 0;

	}
	float **texture = new float*[numvt1];
	for (int i = 0; i < numvt1; i++)
		texture[i] = new float[2];

	int k1 = 0;
	for (int i = 0; i < numvt1; i++)
	{
		texture[i][k1++] = tabvt1[i];
		texture[i][k1++] = tabvt2[i];
		k1 = 0;
	}
	int k2 = 0;
	int numVerNat = numvn1 + numvn2 + numvn3;
	float **normalVer /*naturvert */ = new float*[numvn1];
	for (int i = 0; i < numvn1; i++)
		normalVer[i] = new float[3];
	for (int i = 0; i < numvn1; i++)
	{
		normalVer[i][k2++] = tabvn1[i];
		normalVer[i][k2++] = tabvn22[i];
		normalVer[i][k2++] = tabvn33[i];
		k2 = 0;
	}

	*numtex = numvt1;
	*numnormv = numvn1;
	vert = new float[nfv * 3];
	int end = 0;
	for (int i = 0; i < nfv; i++)
	{
		for (int j = 0; j < 3; j++)
			vert[end++] = vertice[fv[i] - 1][j];
	}
	int end2 = 0;
	tex = new float[nfv * 2];
	for (int i = 0; i < nft; i++)
	{
		for (int j = 0; j < 2; j++)
			tex[end2++] = texture[ft[i] - 1][j];
	}
	normalrvert = new float[nfv * 3];
	int end3 = 0;
	for (int i = 0; i < nfn; i++)
	{
		for (int j = 0; j < 3; j++)
			normalrvert[end3++] = normalVer[fn[i] - 1][j];
	}

	*numver = nfv;
	fclose(file);
}

