#ifndef _FLATMODEL_H_
#define _FLATMODEL_H_

class FlatModel {
public:
	FlatModel(double x = 0, double y = 0, double z = 0, 
		  double width = 0, double height = 0, double depth = 0);
	enum Material { Ground, Wall };
	enum VisibleSurface { Left = 1, Right = 2, Bottom = 4, Top = 8, Front = 16, Back = 32 };
	double x;
	double y;
	double z;
	double width;
	double height;
	double depth;
	Material material;
	int visibleSurfaces;
};

#endif /* _FLATMODEL_H_ */
