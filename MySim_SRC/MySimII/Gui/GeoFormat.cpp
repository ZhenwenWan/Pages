static int geoFormat(int flag){

	/////
	DeleteFile("..\\ini\\box.geo");
	
	QString SaveFile = QString::fromLatin1("../ini/box.geo");
	QFile SFile(SaveFile);
	if (!SFile.open(QIODevice::ReadWrite))
	{

		return 0;
	}
	QTextStream Savestreamfile(&SFile);
	///

	///
	///获取网格控制参数
	QSettings settingf(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	double Freq = settingf.value(QObject::tr("Setup1/SolutionFreq")).toDouble();//获取频率
	QString Hz = settingf.value(QObject::tr("Setup1/SolutionFreq_Unit")).toString();
	//Base::Console().Message("Freq=====================%f\n",Freq);
	if (!Freq){
		Freq = 2;
		Hz = QObject::tr("GHz");
	}
	double Freqs = Freq;
	if (Hz == QObject::tr("GHz")){//
		Freqs = Freq*pow(10, 9);
	}
	if (Hz == QObject::tr("KHz")){//
		Freqs = Freq*pow(10, 3);
	}
	if (Hz == QObject::tr("MHz")){//
		Freqs = Freq*pow(10, 6);
	}
	double wavelength = (3 * pow(10, 8))/Freqs;
	
	QSettings settingss(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	//QString MeshSize1 = settingss.value(QObject::tr("Mesh/totalgrids")).toString();//meshsize1
	QString MeshSize2 = settingss.value(QObject::tr("Mesh/Gridwavelength")).toString();//与波长计算得到meshsize2放入成meshsize4media
	
	//QString MeshSum = settingss.value(QObject::tr("Mesh/totalgrids")).toString();//获取的网格总数
	if (MeshSize2.isEmpty())
	{
		//MeshSize1 = QObject::tr("10");
		MeshSize2 = QObject::tr("10");
		//MeshSum = QObject::tr("1000");
	}
	double ms = MeshSize2.toDouble();
//      double meshsize = (wavelength/ms)*1000;
	double meshsize = ms;
	////
	//Savestreamfile << QObject::tr("Meshsize1=%1;\n").arg(MeshSize1);
	QSettings settingimport(QObject::tr("../ini/importfilename.ini"), QSettings::IniFormat);
	QString namefile=settingimport.value(QObject::tr("Name/filename")).toString();
	QString importflag=settingimport.value(QObject::tr("Name/flag")).toString();
	Savestreamfile << QObject::tr("MeshSizeGlobalCoarsest=%1;\n").arg(meshsize);
	Savestreamfile << QObject::tr("Mesh.CharacteristicLengthMax=MeshSizeGlobalCoarsest;\n");
	if (importflag==QObject::tr("true")){
		Savestreamfile << QObject::tr("Merge \"%1\";\n").arg(namefile);
		Savestreamfile << QObject::tr("SetFactory(\"OpenCASCADE\");\n");
		return 1;
	}
	Savestreamfile << "//+\n";
	Savestreamfile << "SetFactory(\"OpenCASCADE\");\n";
	int shapenum = 0;//记录模型数
	int Point = 0;
	//int sumpoint = 0;//记录点的总数
	//int Edgesum = 0;//总的计数line
	//计数点
	typedef list<CommonFunction::pointnu> pointlist;
	pointlist plist;
	int ppflag = 0;//记录是否是第一次判断点
	pointlist plist2;
	plist2 = plist;
	CommonFunction::pointnu structpoint;
	//记录line数
	typedef list<CommonFunction::lines> lineslist;
	lineslist llist;
	CommonFunction::lines structlines;
	lineslist llist2;
	llist2 = llist;////
	//记录布尔运算
	typedef list<CommonFunction::geometry> geolist;
	geolist geobool;
	CommonFunction::geometry structgeo;
	typedef list<CommonFunction::boolean> boollist;
	boollist bbool;
	CommonFunction::boolean structbool;
	//记录cut的base和Tool
	typedef list<CommonFunction::cut> cutlist;
	cutlist cutbool;
	CommonFunction::cut structcut;

	//记录进行布尔运算的
	typedef list<CommonFunction::geodelete> deletelist;
	deletelist geodelete;
	CommonFunction::geodelete structdelete;
    
	//记录Extrude的编号
	QMap<QString,int> Extrudenum;
	QMap<QString, int> Extrudfaceflag;//11-8记录拉伸出来是平面
    QMap<int,QString> Exmultiflag;//11-12记录是多个面在布尔运算时加入这些面。
	//输出circle和Ellipse
	//vector<QString> VExtrude;
	int ExvNum=0;
	int Exfacenum = 0;
	int facenum = 0;
	QMap<QString,QString> Recordplanenum;//记录单独平面的编号
	QMap<QString,QString> booleanplane;//记录平面的多次布尔运算判断时使用。
    QString ExtrudeDWireface=QObject::tr("");//记录Dwise形成的面数。
	///先遍历平面
	App::Document* pdocs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> psels = pdocs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = psels.begin(); it != psels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
        Base::Console().Message("********1Group==%s\n", Group);
		if(Groups.left(9)==QObject::tr("Rectangle")||Groups.left(7)==QObject::tr("Polygon")){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr("Unable to process rectangle and polygon"));
				return 0;
		}
		if (Groups.left(5) != QObject::tr("Plane") && Groups.left(6) != QObject::tr("Circle") && Groups.left(7) != QObject::tr("Ellipse")&&Groups.left(5)!= QObject::tr("DWire")){
			continue;
		}
		//{// 判断是否不可见的，不可见的跳过
		//	Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		//	bool visibility = true;
		//	if (pcProv) {
		//		visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		//	}
		//	if (visibility == false){// 不可见的跳过
		//		continue;
		//	}
		//}
		int boolean = 0;
		int flagp=0;
		App::GeoFeature* geometry;
		Part::Extrusion *Extrude;
		geometry = static_cast<App::GeoFeature*>(*it);
		const Base::Placement pos = geometry->Placement.getValue();
		double x = pos.getPosition().x;
		double y = pos.getPosition().y;
		double z = pos.getPosition().z;
		double V1 = pos.getRotation()[0];
		double V2 = pos.getRotation()[1];
		double V3 = pos.getRotation()[2];
		double V4 = pos.getRotation()[3];
		double angle;
		Base::Vector3d di;
		pos.getRotation().getValue(di, angle);
		angle = Base::toDegrees<double>(angle);
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			QString bl = QString::fromStdString(bools);
			 // 判断是否不可见的，没有进行布尔运算的Extrude不可写入geobool
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*its);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){//
				flagp=1;
			}
			if (bl.left(4) == QObject::tr("Line")){
				continue;
			}
			Base::Console().Message("********cuts==%s\n", bools);
			//boolean = 2;
			QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			App::DocumentObject *pfaceObj = (*its);
			const TopoDS_Shape& ptShape = static_cast<Part::Feature *>(pfaceObj)->Shape.getValue();
			TopExp_Explorer pexpFace;
			//获取面的个数
			for (pexpFace.Init(ptShape, TopAbs_FACE); pexpFace.More(); pexpFace.Next())
			{
				facenum++;
			}
			//Extrudename = (*its)->getNameInDocument();
			Extrude = static_cast<Part::Extrusion *>(*its);
            App::GeoFeature* Exgeometry;
            Exgeometry = static_cast<App::GeoFeature*>(*its);
            const Base::Placement Expos = Exgeometry->Placement.getValue();
            double Ex = Expos.getPosition().x;
            double Ey = Expos.getPosition().y;
            double Ez = Expos.getPosition().z;
            double EV1 = Expos.getRotation()[0];
            double EV2 = Expos.getRotation()[1];
            double EV3 = Expos.getRotation()[2];
            double EV4 = Expos.getRotation()[3];
            double Eangle;
            Base::Vector3d Edi;
            Expos.getRotation().getValue(Edi, Eangle);
            Eangle = Base::toDegrees<double>(Eangle);
			if (b.left(7) == QObject::tr("Extrude")){
				if (Groups.left(5) == QObject::tr("Plane")){
					Part::Plane *c = static_cast<Part::Plane *>(*it);
					double l = c->Length.getValue();
					double w = c->Width.getValue();
					int r = 0;
					QString box = QObject::tr("Rectangle(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(r);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Surface{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					//VExtrude.push_back(Extrude);
					++shapenum;
					Savestreamfile << Extrude;
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 0;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 0);
						flagp = 0;
					}
					continue;
				}

				if (Groups.left(6) == QObject::tr("Circle")){
					Part::Circle *c = static_cast<Part::Circle *>(*it);
					double Rc=c->Radius.getValue();
					double a1 = c->Angle0.getValue();
					double a2 = c->Angle1.getValue();
					QString box = QObject::tr("Circle(%1) = {%2, %3, %4, %5, %6*Pi, %7*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(Rc).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					Savestreamfile << Extrude;
					++shapenum;
					//VExtrude.push_back(Extrude);
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 1);
						flagp = 0;
					}
					continue;
				}
				if (Groups.left(7) == QObject::tr("Ellipse")){
					Part::Ellipse *c = static_cast<Part::Ellipse *>(*it);
					double a1=c->Angle0.getValue();
					double a2=c->Angle1.getValue();
					double R1=c->MajorRadius.getValue();
					double R2 = c->MinorRadius.getValue();
					QString box = QObject::tr("Ellipse(%1) = {%2, %3, %4, %5, %6, %7*Pi, %8*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					Savestreamfile << Extrude;
					++shapenum;
					//VExtrude.push_back(Extrude);
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 1);
						flagp = 0;
					}
					continue;
				}
                if (Groups.left(5) == QObject::tr("DWire")){//布尔运算里面2018-11-6
					App::DocumentObject *faceObj = (*it); //->Shape.getValue();
					const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
					TopExp_Explorer countvertex;
					int pointssum = 0;
					int num = facenum;
					int pointnm = 0;
					for (countvertex.Init(tShape, TopAbs_VERTEX); countvertex.More(); countvertex.Next())
					{
						pointssum++; 
						TopoDS_Vertex  vtx = TopoDS::Vertex(countvertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						if (pointssum % 2 == 1 && pointssum != 1){
							Base::Console().Message("********points=%d====x=%fy=%fz=%f\n", pointssum, pt.X(), pt.Y(), pt.Z());
						}
						else{
							QString point = QObject::tr("Point(%1) = {%2, %3, %4, %5};\n").arg(++facenum).arg(pt.X()).arg(pt.Y()).arg(pt.Z()).arg(pointssum);
							Savestreamfile << point;
							pointnm++;
						}
					}
					int linenum = facenum;
					QString surfacenum = QObject::tr("");
                    QString multinum = QObject::tr("");
					for (int i = ++num; i <= facenum;){
						if (i != (facenum - 1))
                        { 
                          ExtrudeDWireface=ExtrudeDWireface+QObject::tr("%1,").arg(++shapenum);
                          multinum=multinum+QObject::tr("%1,").arg(shapenum);
                        }
						QString line = QObject::tr("Line(%1) = {%2, %3};\n").arg(++linenum).arg(i++).arg(i);
						Savestreamfile << line;
                       // QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Line{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(surfacenum.left(surfacenum.length() - 1));
					    // Savestreamfile << Rotation;
						surfacenum = surfacenum + QObject::tr("%1,").arg(linenum);
						if (i == facenum){ break; }
					}
					facenum = facenum + pointnm+1;//编号得多加，不然后面会显示有的点或者line编号已存在。
                    Base::Console().Message("********ExtrudeDWireface=%s\n", ExtrudeDWireface.toStdString().c_str());
					QString Rotation = QObject::tr("Translate { %1,%2,%3} { Line{%4}; }\n").arg(Ex).arg(Ey).arg(Ez).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Rotation;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%1, %2, %3} { Line{%4}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Extrude;
                    ++shapenum;
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8,%9}; }\n").arg(Edi.x).arg(Edi.y).arg(Edi.z).arg(Ex).arg(Ey).arg(Ez).arg(Eangle / 180).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(shapenum);
                   // QString Rotation = QObject::tr("Translate { %1,%2,%3} { Surface{%8,%9}; }\n").arg(Ex).arg(Ey).arg(Ez).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(shapenum);
					//Savestreamfile << Rotation;
                    Exmultiflag.insert(shapenum,multinum);
                    boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl,1);
						flagp = 0;
					}
					continue;
                }
                
			}
			

		}//for 布尔运算
		
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false&&boolean!=2){// 不可见没有进行布尔运算的面的跳过
				continue;
			}
		}
		if (boolean == 3){
			continue;
		}
		if (boolean == 0){
			structgeo.boolean = "1";
			structgeo.geom = Group;
			structgeo.boole = "1";
		}
		///////////先保存平面
		if (Groups.left(5) == QObject::tr("Plane")){
					Part::Plane *c = static_cast<Part::Plane *>(*it);
					double l = c->Length.getValue();
					double w = c->Width.getValue();
					int r = 0;
					QString box = QObject::tr("Rectangle(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(r);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
		if (Groups.left(6) == QObject::tr("Circle")){
					Part::Circle *c = static_cast<Part::Circle *>(*it);
					double Rc=c->Radius.getValue();
					double a1 = c->Angle0.getValue();
					double a2 = c->Angle1.getValue();
					QString box = QObject::tr("Circle(%1) = {%2, %3, %4, %5, %6*Pi, %7*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(Rc).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
		if (Groups.left(7) == QObject::tr("Ellipse")){
					Part::Ellipse *c = static_cast<Part::Ellipse *>(*it);
					double a1=c->Angle0.getValue();
					double a2=c->Angle1.getValue();
					double R1=c->MajorRadius.getValue();
					double R2 = c->MinorRadius.getValue();
					QString box = QObject::tr("Ellipse(%1) = {%2, %3, %4, %5, %6, %7*Pi, %8*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
        if (Groups.left(5) == QObject::tr("DWire")){//2018-11-6
             App::DocumentObject *faceObj = (*it); //->Shape.getValue();
					const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
					TopExp_Explorer countvertex;
					int pointssum = 0;
					int num = facenum;
					for (countvertex.Init(tShape, TopAbs_VERTEX); countvertex.More(); countvertex.Next())
					{
						pointssum++; 
						TopoDS_Vertex  vtx = TopoDS::Vertex(countvertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						if (pointssum % 2 == 1 && pointssum != 1){
							//Base::Console().Message("********points=%d====x=%fy=%fz=%f\n", pointssum, pt.X(), pt.Y(), pt.Z());
						}
						else{
							QString point = QObject::tr("Point(%1) = {%2, %3, %4, %5};\n").arg(++facenum).arg(pt.X()).arg(pt.Y()).arg(pt.Z()).arg(pointssum);
							Savestreamfile << point;
						}
					}
					int linenum = facenum;
					QString surfacenum = QObject::tr("");
					for (int i = ++num; i <= facenum;){
						QString line = QObject::tr("Line(%1) = {%2, %3};\n").arg(++linenum).arg(i++).arg(i);
						Savestreamfile << line;
						surfacenum = surfacenum + QObject::tr("%1,").arg(linenum);
						if (i == facenum){ break; }
					}
                    QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Line{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
					//Base::Vector3d v = Extrude->Dir.getValue();
					//QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(surfacenum.left(surfacenum.length() - 1));
					//Savestreamfile << Extrude;       
        }
		structgeo.num = facenum;
		structgeo.m = facenum;
		structgeo.plan = 1;
		geobool.push_back(structgeo);
			
	}//for

	/*vector<QString>::iterator itV;
	for (itV = VExtrude.begin(); itV != VExtrude.end(); ++itV){
		Savestreamfile << *itV;
	}*/
	//遍历几何体
	
	App::Document* docs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		int boolean = 0;
		int bol= 0;
		int error=0;//判断是否能够写成geo
		QString Groups = QString::fromStdString(Group);
		//Base::Console().Message("Groups======%s\n", Group.c_str());
		if ((Groups.left(3) == QObject::tr("Cut")) || (Groups.left(6) == QObject::tr("Common")) || (Groups.left(6) == QObject::tr("Fusion"))){
			//Base::Console().Message("line1======%s\n", Group);
			boolean = 1;
			bol++;
			error++;
			//continue;
		}
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		

		//{// 判断是否不可见的，不可见的跳过
		//	Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		//	bool visibility = true;
		//	if (pcProv) {
		//		visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		//	}
		//	if (visibility == false){// 不可见的跳过
		//		continue;
		//	}
		//}
		///////////////////////////		
		/////////////////////////////////////////////

		//Base::Console().Message("line======%s\n", Group);
		int visibilityflag = 0;
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			QString bl = QString::fromStdString(bools);
			if (bl.left(4) == QObject::tr("Line")){
				continue;
			}
			//Base::Console().Message("********boolean==%s\n", bools.c_str());
			boolean = 2;
			bol++;
			QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				visibilityflag = 1;
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				continue;
			}


		}//for 布尔运算

		if(visibilityflag==0){// 判断是否不可见的，没有进行布尔运算的不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
			if (Groups.left(7) == QObject::tr("Extrude")){
				continue;
			}
		}
		
		//
		//Base::Console().Message("********V1*******************\n");
		if (Groups.left(3) == QObject::tr("Cut")){
			App::DocumentObject *Obj = (*it);
			Part::Cut *c = static_cast<Part::Cut *>(Obj);
			App::DocumentObject *a = c->Base.getValue();
			std::string base = a->getNameInDocument();
			App::DocumentObject *aa = c->Tool.getValue();
			std::string tool = aa->getNameInDocument();
			//Base::Console().Message("===========4=======Base====%s========\n", base);
			//Base::Console().Message("==========3========Tool=====%s======\n", tool);
			structcut.cutname = Group;
			structcut.basename = base;
			structcut.toolname;
			cutbool.push_back(structcut);
		}
		if (boolean == 1){
			continue;
		}
		if(bol==2){
			continue;
		}
		if (boolean == 0){
			structgeo.boolean = "1";
			structgeo.geom = Group;
			structgeo.boole = "1";
		}

		//Base::Console().Message("================================\n");
		App::GeoFeature* geometry = static_cast<App::GeoFeature*>(*it);
		const Base::Placement pos = geometry->Placement.getValue();
		double x = pos.getPosition().x;
		double y = pos.getPosition().y;
		double z = pos.getPosition().z;
		double V1 = pos.getRotation()[0];
		double V2 = pos.getRotation()[1];
		double V3 = pos.getRotation()[2];
		double V4 = pos.getRotation()[3];
		double angle;
		Base::Vector3d di;
		pos.getRotation().getValue(di, angle);
		angle = Base::toDegrees<double>(angle);
		//Base::Console().Message("********V2*******************\n");
		int bflag = 0;//记录布尔下面还有布尔运算
		if (Groups.left(3) == QObject::tr("Box")){
			bflag = 1;
			Part::Box *c = static_cast<Part::Box *>(*it);
			double h = c->Height.getValue();
			double w = c->Width.getValue();
			double l = c->Length.getValue();
			QString box = QObject::tr("Box(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(h);
			Savestreamfile << box;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle/180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(8) == QObject::tr("Cylinder")){
			bflag = 1;
			Part::Cylinder *c = static_cast<Part::Cylinder *>(*it);
			double h = c->Height.getValue();
			double R = c->Radius.getValue();
			double a = c->Angle.getValue();
			QString sphere = QObject::tr("Cylinder(%1) = {%2, %3, %4, 0, 0, %5, %6, %7*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(h).arg(R).arg(a / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(4) == QObject::tr("Cone")){
			bflag = 1;
			Part::Cone *c = static_cast<Part::Cone *>(*it);
			double a = c->Angle.getValue();
			double R1 = c->Radius1.getValue();
			double R2 = c->Radius2.getValue();
			double h = c->Height.getValue();
			QString Cone = QObject::tr("Cone(%1) = { %2, %3, %4, 0, 0, %5, %6, %7, %8*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(h).arg(R1).arg(R2).arg(a / 180);
			Savestreamfile << Cone;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(6) == QObject::tr("Sphere")){
			bflag = 1;
			Part::Sphere *c = static_cast<Part::Sphere *>(*it);
			double A1 = c->Angle1.getValue();
			double A2 = c->Angle2.getValue();
			double A3 = c->Angle3.getValue();
			double R = c->Radius.getValue();
			QString sphere = QObject::tr("Sphere(%1) = {%2, %3, %4, %5, Pi*%6, Pi*%7, %8*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(R).arg(A1 / 180).arg(A2 / 180).arg(A3 / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(5) == QObject::tr("Torus")){
			bflag = 1;
			Part::Torus *c = static_cast<Part::Torus *>(*it);
			double R1 = c->Radius1.getValue();
			double R2 = c->Radius2.getValue();
			double A1 = c->Angle1.getValue();
			double A2 = c->Angle2.getValue();
			double A3 = c->Angle3.getValue();
			QString sphere = QObject::tr("Torus(%1) = {%2, %3, %4, %5, %6, %7*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(A3 / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(7) == QObject::tr("Extrude")){//...........
			QMap<QString, int>::Iterator Ex;
			QMap<QString, int>::Iterator Exfaceflag;
			Ex=Extrudenum.find(Groups);
			Exfaceflag=Extrudfaceflag.find(Groups);
			structgeo.num = Ex.value();
			structgeo.m = Ex.value();
			structgeo.plan = Exfaceflag.value();
			geobool.push_back(structgeo);
			error++;
			continue;
		}
		if(error==0){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr("Geometric content that does not conform to GEO processing"));
				return 0;
		}
		if (bflag == 0){
			structgeo.num = ++shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
		}


	}//for 遍历工程树
	//Base::Console().Message("********V3*******************\n");
	
	///////////////2018-6-11////////
	//int bo= 0;
	//int boo=0;
	App::Document* d = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> se = d->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = se.begin(); it != se.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		int bo= 0;
		int boo=0;
		QString Groups = QString::fromStdString(Group);
		if ((Groups.left(3) == QObject::tr("Cut")) || (Groups.left(6) == QObject::tr("Common")) || (Groups.left(6) == QObject::tr("Fusion"))){
			//Base::Console().Message("line1======%s\n", Group);
			bo= 1;
			//continue;
		}
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}

		int visibilityflag = 0;
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		QString b;
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			b = QString::fromStdString(bools);
			if (b.left(4) == QObject::tr("Line")){
				continue;
			}
			//Base::Console().Message("********cuts==%s\n", bools);
			boo = 1;
			//boolean2=2;
			//QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				visibilityflag = 1;
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				continue;
			}


		}//for 布尔运算

		if(visibilityflag==0){// 判断是否不可见的，没有进行布尔运算的不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		//
		//Base::Console().Message("********V1*******************\n");
		if (Groups.left(3) == QObject::tr("Cut")){
			App::DocumentObject *Obj = (*it);
			Part::Cut *c = static_cast<Part::Cut *>(Obj);
			App::DocumentObject *a = c->Base.getValue();
			std::string base = a->getNameInDocument();
			App::DocumentObject *aa = c->Tool.getValue();
			std::string tool = aa->getNameInDocument();
			//Base::Console().Message("===========4=======Base====%s========\n", base);
			//Base::Console().Message("==========3========Tool=====%s======\n", tool);
			structcut.cutname = Group;
			structcut.basename = base;
			structcut.toolname;
			cutbool.push_back(structcut);
		}
		/*if (boolean == 1){
			continue;
		}*/
		
		if(boo&&bo){
			structgeo.num = ++shapenum;
			structgeo.m = shapenum;
			if (true == booleanplane.contains(Groups)){
			   structgeo.plan = 1;
			   booleanplane.insert(b,Groups);
			}else{
			structgeo.plan = 0;
			}
			geobool.push_back(structgeo);
		}
		
	}
	
	/////////////////////////////////////
	list<CommonFunction::geometry>::iterator it;
	list<CommonFunction::geometry>::iterator its;
	list<CommonFunction::geometry>::iterator ita;
	list<CommonFunction::geometry>::iterator it2;
	list<CommonFunction::geometry>::iterator it3;
	list<CommonFunction::geometry>::iterator it4;
	list<CommonFunction::cut>::iterator itc;//cut
	geolist geobools,geoboolss;
	geobools = geobool;
	geoboolss = geobool;
	//geolist geoover;
	//CommonFunction::geometry structgeol;//最终输出时剪切使用
	int i = 0;//判断只能写入一次
	int shapenum1, shapenum2, shapenum3;
	int boonum = 0;
	//int flnum = 1;//判断是否第一次使用编号
	//int fcnum = 1;//判断是否第一次使用编号
	//int fonum = 1;//
	int planeflag = 0;//判断是平面的
	int firstplan=0;//记录平面是否是第一次循环
	QMap<string, int> plannummap;
	//int numover = 0;//记录最终的编号
	/*  for (it = geobools.begin(); it != geobools.end();++it){
		Base::Console().Message("==1==%s=====%s====%s====%d========%d==%d===\n", (*it).boolean, (*it).geom, (*it).boole,(*it).num, (*it).m,(*it).plan);
	} 
	return 0; */
	QMap<QString,QString> geounmap;
	for (it = geobools.begin(); it != geobools.end();){
		//Base::Console().Message("==1==%s=====%s====%s====%d========%d===plan=%d=\n", (*it).boolean, (*it).geom, (*it).boole,(*it).num, (*it).m, (*it).plan);
		int fcnum = 1;//判断是否第一次使用编号
		int flnum = 1;
		int fonum = 1;
		int unionflag=0;//union判断接触时判断是否是多个进行的union
		//Base::Console().Message("********V4*******************\n");
		QString Groups = QString::fromStdString((*it).geom);
		int brk=0;//退出本次循环
		if ((*it).plan){
			planeflag = 1;
			
		}
		else{
			planeflag = 0;
		}
		if (firstplan==0){
				plannummap.insert((*it).boolean, (*it).m);
				firstplan = 1;
			}
		else{
			if (true==plannummap.contains((*it).boolean)){//平面的面可能与体相同的号，判断不出大小，使用这个防止多次进行输出。
				it++;
				continue;
				
				}
			else{
					plannummap.insert((*it).boolean, (*it).m);
				}
			}
		
		int s1=0;
		QString b = QString::fromStdString((*it).geom);
		if(b.left(6)==QObject::tr("Fusion")||b.left(3)==QObject::tr("Cut")||b.left(6)==QObject::tr("Common"))//平面加不加s
		{
			s1=1;
		}
		string s = (*it).boolean;
		shapenum1 = (*it).num;
		shapenum2 = (*it).num;
		shapenum3 = (*it).num;
		string gm = (*it).geom;
		int flagits=0;//记录++
		for (its = geoboolss.begin(); its != geoboolss.end();){
			//Base::Console().Message("********V899*******************\n");
			//Base::Console().Message("    ==2==%s=====%s========%d=====%d==plan=%d\n", (*its).boolean, (*its).geom, (*its).num, (*its).m,(*its).plan);
			if ((*its).boole == "1"){
				its++;
				continue;
			}
			int s2=0;
				QString bb = QString::fromStdString((*its).geom);
				if(bb.left(6)==QObject::tr("Fusion")||bb.left(3)==QObject::tr("Cut")||bb.left(6)==QObject::tr("Common"))//平面加不加s
				{
					s2=1;
				}
			//Base::Console().Message("********V111*****s1=%d,s2=%d**************\n",s1,s2);
			bool T=((s == (*its).boolean) &&((*its).geom != gm) && ((*its).m>(*it).m));//正常体的
			bool TP= ((s == (*its).boolean) &&((*its).geom != gm) && planeflag);//平面的运算的，平面编号可能会与体相同
	        if(T||TP){
				
				//Base::Console().Message("********8741*******************\n");
			//}
			//if((s == (*its).boolean) &&((*its).geom != gm) && ((*its).m>(*it).m)){
				//Base::Console().Message("********V88*******************\n");
				if ((*it).boole == "fusion"){
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							if (flnum){
								if (planeflag==1){//平面读取更新编号
									//Base::Console().Message("********V9915*******%d****%d********\n",(*it2).num,shapenum1);
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									flnum = 0;
									//Base::Console().Message("********V9915*******%d************\n",(*it2).num);
									////2018-6-12///cut没有使用
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
									
								}
								else{//获取体的编号
									boonum = (*it2).num;
									flagit = 1;
									flnum = 0;
								}
							}
							else{
								if (planeflag==1){
									//Base::Console().Message("********Vnj*******************\n");
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum1;
									(*it).plan = 1;
									it = it4;
									boonum = ++shapenum;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}else{
									//Base::Console().Message("********V23145*******************\n");
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end();ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}

						}
					}
					if (flagit == 0){//没有这个的布尔运算说明它是最后一个。
						++shapenum;
						if (planeflag){
							if ((*its).plan){//两个都是平面
								//Base::Console().Message("********V2*******************\n");
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum1));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
			                        Multiflag1=Exmultiflag.find(shapenum1);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum1).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
                                    }
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
							}
							else{//一个是平面
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1){
								common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number(shapenum1));
							}
						}
						else{//两个都是体的
							if ((*its).plan){//一个是平面，布尔运算的面与体结合时会出现这种情况。eml体与面操作会出问题,只能按面union体
							    QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s2==1){
								common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg((*its).num).arg(shapenum1);
								QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg((*its).num).arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{//....
								QString fusion = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								//Savestreamfile << fusion;
								QString m;
								if(shapenum1!=(*it).num){//说明它是多个union
									m=QObject::tr("munion");
								}else{
									m = QString::fromStdString((*it).geom);
								}
								//Base::Console().Message("********V23584*******************\n");
								bool un=CommonFunction::Booleanunionoperation((*it).geom,(*its).geom);
								if(unionflag==1){//说明它多个union第一个就是接触的。
									un=true;
									//Base::Console().Message("********V2954*******************\n");
								}
								if(unionflag==2){
									un=false;
								}
								if(un){
									structbool.str = fusion.toStdString();
									structbool.bnum = shapenum;
									structbool.Booleanname=(*it).boolean;
									bbool.push_back(structbool);
									structdelete.str = (*it).boolean;
									structdelete.num1 = shapenum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
									unionflag=1;
								}
								else{
									//Base::Console().Message("********V18845*******************\n");
									QString unstr = QObject::tr("%1,%2,%3,%4,%5").arg(shapenum).arg(m).arg(shapenum1).arg(QString::fromStdString((*its).geom)).arg((*its).num);
									QString namenum = QObject::tr("%1,%2").arg(QString::fromStdString((*it).boolean)).arg(shapenum);
									geounmap.insert(namenum,unstr);
									unionflag=2;
								}
							}
						}
							shapenum1 = shapenum;
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
							}
							else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
					}
					else{
						if (planeflag){
							if ((*its).plan){//第一级平面的布尔运算
							//Base::Console().Message("********V1**********%d*********\n",boonum);
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum1));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
			                        Multiflag1=Exmultiflag.find(shapenum1);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum1).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum1));
								}
							//QString common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = boonum;
							structbool.Booleanname = "Plane," + (*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum3;
							structdelete.str ="Plane," + (*it).boolean;
							structdelete.num1 = boonum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							//QString snums = QObject::tr("s%1s").arg(boonum);
							//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							//Recordplanenum.remove(QString::number((*its).num));
							//Recordplanenum.remove(QString::number(shapenum1));
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s1==1){
								common= QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common= QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(boonum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number(shapenum1));
							}
						}
						else{//第一级体的布尔运算
							if ((*its).plan){
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s2==1){
								common= QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum1);
								QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common= QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								QString fusion = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								//Savestreamfile << fusion;
								QString m;
								if(shapenum1!=(*it).num){//说明它是多个union
									m=QObject::tr("munion");
								}else{
									m = QString::fromStdString((*it).geom);
								}
								//Base::Console().Message("********V217*******************\n");
								bool un=CommonFunction::Booleanunionoperation((*it).geom,(*its).geom);//判断是否接触的，不是重新处理。
								if(unionflag==1){//说明它多个union第一个就是接触的。
									un=true;
									//Base::Console().Message("********Vn*******************\n");
								}
								if(unionflag==2){
									un=false;
								}
							    if(un){
									structbool.str = fusion.toStdString();
									structbool.bnum = boonum;
									structbool.Booleanname=(*it).boolean;
									bbool.push_back(structbool);
									//numover = shapenum1;
									structdelete.str = (*it).boolean;
									structdelete.num1 = boonum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
									unionflag=1;
								}
								else{
									//Base::Console().Message("********V23mi*******************\n");
									QString unstr = QObject::tr("%1,%2,%3,%4,%5").arg(boonum).arg(m).arg(shapenum1).arg(QString::fromStdString((*its).geom)).arg((*its).num);
									QString namenum = QObject::tr("%1,%2").arg(QString::fromStdString((*it).boolean)).arg(boonum);
									geounmap.insert(namenum,unstr);
									unionflag=2;
								}
							}
						}
							shapenum1 = boonum;
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
							}
							else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
					}
				}
				else if ((*it).boole == "cut"){
					//Base::Console().Message("********V*******************\n");
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							//Base::Console().Message("********Vxx*******************\n");
							if (fcnum){
								if (planeflag==1){
								//	Base::Console().Message("********V9915*******************\n");
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									fcnum = 0;
									
									////2018-6-12///cut没有使用
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = (*it2).num;
									flagit = 1;
									fcnum = 0;
								}
							}
							else{
								if (planeflag==1){
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum2;
									(*it).plan = 1;
									it = it4;
									////2018-6-12///
									boonum = ++shapenum;
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}
						}
					}
					if (flagit == 0){//多级布尔运算的
						//Base::Console().Message("********Vg*******************\n");
						++shapenum;
						if (planeflag==1){
							if ((*its).plan){
							//Base::Console().Message("********V5*******************\n");
							for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
									if ((*itc).cutname == (*its).boolean){
										if ((*itc).basename == (*its).geom){
										     QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find((*its).num);
                                                Multiflag2=Exmultiflag.find(shapenum2);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg((*its).num).arg(shapenum2);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
											//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
											structbool.str = common.toStdString();
											structbool.bnum = shapenum;
											structbool.Booleanname = "Plane," + (*it).boolean;
											bbool.push_back(structbool);
											//numover = shapenum2;
											structdelete.str = "Plane," +(*it).boolean;
											structdelete.num1 = shapenum;
											structdelete.num2 = (*its).num;
											geodelete.push_back(structdelete);
											/* QString snums = QObject::tr("s%1s").arg(shapenum);
											Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
											Recordplanenum.remove(QString::number((*its).num));
											Recordplanenum.remove(QString::number(shapenum2)); */
											break;
										}
										else{
											 QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find(shapenum2);
                                                Multiflag2=Exmultiflag.find((*its).num);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum2).arg((*its).num);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
											//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
											structbool.str = common.toStdString();
											structbool.bnum = shapenum;
											structbool.Booleanname = "Plane," + (*it).boolean;
											bbool.push_back(structbool);
											//numover = shapenum2;
											structdelete.str = "Plane," +(*it).boolean;
											structdelete.num1 = shapenum;
											structdelete.num2 = (*its).num;
											geodelete.push_back(structdelete);
											/* QString snums = QObject::tr("s%1s").arg(shapenum);
											Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
											Recordplanenum.remove(QString::number((*its).num));
											Recordplanenum.remove(QString::number(shapenum2)); */
											break;
										}
									}
							}//for base tool
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								QString cut;
								if(s1==1){
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum2));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum2));
							}
						}
						else{//体的布尔运算
							//++shapenum;
							//Base::Console().Message("********V6*******************\n");
							if ((*its).plan){//只能面剪切体
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								QString cut;
								if(s2==1){
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
							for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								if ((*itc).cutname == (*its).boolean){
									if ((*itc).basename == (*its).geom){
										//Base::Console().Message("********V7*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
										structbool.str = cut.toStdString();
										structbool.bnum = shapenum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum;
										structdelete.str = (*it).boolean;
										structdelete.num2 = shapenum;
										structdelete.num1 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
									else{
										//Base::Console().Message("********V8*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
										structbool.str = cut.toStdString();
										structbool.bnum = shapenum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum;
										//Base::Console().Message("********V81*******************\n");
										structdelete.str = (*it).boolean;
										structdelete.num1 = shapenum;
										structdelete.num2 = (*its).num;
										//Base::Console().Message("********V82*******************\n");
										geodelete.push_back(structdelete);
										break;
									}
								}
							}
							}
						}
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
						    }
						    else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
						//Base::Console().Message("********V86*******************\n");
					}
					else{
						if (planeflag==1){
							//Base::Console().Message("********V9*******************\n");
							if ((*its).plan){
								for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								//Base::Console().Message("********V956*******************\n");
									if ((*itc).cutname == (*its).boolean){
										if ((*itc).basename == (*its).geom){
											QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find((*its).num);
                                                Multiflag2=Exmultiflag.find(shapenum2);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg((*its).num).arg(shapenum2);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
										//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
										structbool.str = common.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname = "Plane," + (*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum2;
										structdelete.str = "Plane," +(*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										//QString snums = QObject::tr("s%1s").arg(boonum);
										//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
										//Recordplanenum.remove(QString::number((*its).num));
										//Recordplanenum.remove(QString::number(shapenum2));
										break;
										}
										else{
											QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find(shapenum2);
                                                Multiflag2=Exmultiflag.find((*its).num);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum2).arg((*its).num);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
										//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
										structbool.str = common.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname = "Plane," + (*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum2;
										structdelete.str = "Plane," +(*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										//QString snums = QObject::tr("s%1s").arg(boonum);
										//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
										//Recordplanenum.remove(QString::number((*its).num));
										//Recordplanenum.remove(QString::number(shapenum2));
										break;
										}
									}
								}
							}
							else{
								QString cut; 
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								if(s1==1){
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(shap1);
								}
								else{
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(QString::number(shapenum2));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum2));
							}
						}
						else{
							if ((*its).plan){
								QString cut; 
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								if(s2==1){
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(shap1);
								}
								else{
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								if ((*itc).cutname == (*its).boolean){
									if ((*itc).basename == (*its).geom){
										//Base::Console().Message("********V10*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
										structbool.str = cut.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = boonum;
										structdelete.str = (*it).boolean;
										structdelete.num2 = boonum;
										structdelete.num1 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
									else{
										//Base::Console().Message("********V11*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
										structbool.str = cut.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = boonum;
										structdelete.str = (*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
								}
								}
							}
						}
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
						    }
						    else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
						    }
						//}

					}
					//Base::Console().Message("********V87*******************\n");
				}
				else if ((*it).boole == "common"){
					//////
					//Base::Console().Message("********8742*******************\n");
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							//Base::Console().Message("********8743*******************\n");
							if (fonum){
								if (planeflag){
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									fonum = 0;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
									
								}
								else{
									boonum = (*it2).num;
									flagit = 1;
									fonum = 0;
								}

							}
							else{
								if (planeflag){
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum3;
									(*it).plan = 1;
									it = it4;
									////2018-6-12///
									boonum = ++shapenum;
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}
						}
					}
					if (flagit == 0){
						//Base::Console().Message("********8744*******************\n");
						++shapenum;
						if (planeflag){
							if ((*its).plan){
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum3));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                    Multiflag1=Exmultiflag.find(shapenum3);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                       common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum3).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								// QString common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number((*its).num));
								//Recordplanenum.remove(QString::number(shapenum3));
							}
							else{
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1){
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							
								//Recordplanenum.remove(QString::number(shapenum3));
							}
						}
						else{
							//Base::Console().Message("********8745*******************\n");
							if ((*its).plan){
									QString common;
									QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s2==1){
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum3);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
									structbool.str = common.toStdString();
									structbool.bnum = shapenum;
									structbool.Booleanname = "Plane," + (*it).boolean;
									bbool.push_back(structbool);
									//numover = shapenum3;
									structdelete.str = "Plane," +(*it).boolean;
									structdelete.num1 = shapenum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
							}
							//++shapenum;
							else{
							QString common = QObject::tr("BooleanIntersection(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = shapenum;
							structbool.Booleanname=(*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum;
							structdelete.str = (*it).boolean;
							structdelete.num1 = shapenum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							}
						}
						shapenum3 = shapenum;
						if (it == its){
							its++;
							geobools.erase(it++);
							flagits=1;
						}
						else{
							it3 = it;
							it = its++;
							geobools.erase(it);//删除已经输出的
							it = it3;
						}

					}
					else{
						//Base::Console().Message("********8746*******************\n");
						if (planeflag){
							if ((*its).plan){
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum3));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                    Multiflag1=Exmultiflag.find(shapenum3);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                       common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum3).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum3));
								}
							//QString common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = boonum;
							structbool.Booleanname = "Plane," + (*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum3;
							structdelete.str = "Plane," +(*it).boolean;
							structdelete.num1 = boonum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							//QString snums = QObject::tr("s%1s").arg(boonum);
							//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							//Recordplanenum.remove(QString::number((*its).num));
							//Recordplanenum.remove(QString::number(shapenum3));
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s1==1){
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}
								else{
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum3));
							}
						}
						else{
							if ((*its).plan){
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s2==1){
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum3);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}
								else{
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								QString common = QObject::tr("BooleanIntersection(%1)={ Volume{%2};Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname=(*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
						}
						shapenum3 = boonum;
						if (it == its){
							its++;
							geobools.erase(it++);
							flagits=1;
						}
						else{
							it3 = it;
							it = its++;
							geobools.erase(it);//删除已经输出的
							it = it3;
						}


					}
				}
				//Base::Console().Message("********V88*******************\n");
			}//if
			else{
				its++;
			}
			//Base::Console().Message("********V89*******************\n");
			//Base::Console().Message("********V3333*******************\n");
			//else{
			//++its;
			//}
		}//its for
		if(flagits!=1){it++;}
	}//it for
	//Base::Console().Message("********V12*******************\n");
	
	 if(!geounmap.isEmpty()){//输出多个union的内容
		QString slist;
		QString boollean1, boollean2;
		int m,n=0;
		QStringList mlist;
		
		//Base::Console().Message("******2********%d*\n",geounmap.size());
		for (QMap<QString, QString>::iterator iter = geounmap.begin(); iter != geounmap.end();)
		{
			//Base::Console().Message("******1*********\n");
				int i = 0;
				boollean1 = iter.key().section(QObject::tr(","), 0, 0);
				m = iter.key().section(QObject::tr(","), 1, 1).toInt();
				QString geoname1 = iter.value().section(QObject::tr(","), 1, 1);
				int geonum1=iter.value().section(QObject::tr(","), 2, 2).toInt();
				QString geoname2 = iter.value().section(QObject::tr(","), 3, 3);
				int geonum2=iter.value().section(QObject::tr(","), 4, 4).toInt();
				//Base::Console().Message("******1****d=%d**s=%d**m=%d********==%s\n",geonum1,geonum2,m,boollean1.toStdString().c_str());
				if (geounmap.size() == 1){//925只有一个需要直接输出。
					QString fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
					structbool.str = fusion1.toStdString();
					structbool.bnum = m;
					structbool.Booleanname = boollean1.toStdString();
					bbool.push_back(structbool);
					structdelete.str = boollean1.toStdString();
					structdelete.num1 = m;
					structdelete.num2 = geonum2;
					geodelete.push_back(structdelete);
					break;
				}//end925
				for (QMap<QString, QString>::iterator ites = geounmap.begin(); ites != geounmap.end();)
				{
					boollean2 = ites.key().section(QObject::tr(","), 0, 0);
					n = ites.key().section(QObject::tr(","), 1, 1).toInt();
					
					if (boollean2 == boollean1){ 
						if(m==n){ites++;continue; }
						QString geoname3 = ites.value().section(QObject::tr(","), 1, 1);
						int geonum3 = ites.value().section(QObject::tr(","), 2, 2).toInt();
						QString geoname4 = ites.value().section(QObject::tr(","), 3, 3);
						int geonum4 = ites.value().section(QObject::tr(","), 4, 4).toInt();
						//Base::Console().Message("******5641******%s***%s***%s*******%s\n",geoname1.toStdString().c_str(),geoname2.toStdString().c_str(),geoname3.toStdString().c_str(),geoname4.toStdString().c_str());
						QString fusion1;
						QString fusion2;
						bool tf1 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname2.toStdString());
						if (tf1){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum3).arg(geonum4);
						// Base::Console().Message("******5641****1\n");
						}
						bool tf2 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname3.toStdString());
						if (tf2){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum3);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum2).arg(geonum4);
						 //Base::Console().Message("******5641****2\n");
						}
						bool tf3 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname4.toStdString());
						if (tf3){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum4);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum2).arg(geonum3);
						 //Base::Console().Message("******5641****3\n");
						}
						bool tf4 = CommonFunction::Booleanunionoperation(geoname2.toStdString(), geoname3.toStdString());
						if (tf4){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum2).arg(geonum3);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum1).arg(geonum4);
						 //Base::Console().Message("******5641****4\n");
						}
						bool tf5 = CommonFunction::Booleanunionoperation(geoname2.toStdString(), geoname4.toStdString());
						if (tf5){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum2).arg(geonum4);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum1).arg(geonum3);
						 //Base::Console().Message("******5641****5\n");
						}
						if(i!=1){
							fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum3).arg(geonum4);
						}
						structbool.str = fusion1.toStdString();
						structbool.bnum = m;
						structbool.Booleanname = boollean1.toStdString();
						bbool.push_back(structbool);			
						structdelete.str = boollean1.toStdString();
						structdelete.num1 = m;
						structdelete.num2 = geonum2;
						geodelete.push_back(structdelete);
						
						structbool.str = fusion2.toStdString();
						structbool.bnum = n;
						structbool.Booleanname = boollean1.toStdString();
						bbool.push_back(structbool);			
						structdelete.str = boollean1.toStdString();
						structdelete.num1 = n;
						structdelete.num2 = geonum4;
						geodelete.push_back(structdelete);
						ites=geounmap.erase(ites);
						iter=geounmap.erase(iter);
						i=2;
						break;//两两能组成结合的可以。
					}
					else{
						ites++;
					}
				}//ites
			if(i!=2){
				iter++;
			}
				
		}//for iter
		
		
	}
	//输出布尔过程
	list<CommonFunction::boolean>::iterator bit;
	list<CommonFunction::boolean>::iterator bits;
	bbool.sort();

	//boollist bbool2;
	int bbnum=bbool.size();
	//int bitnum = 0;
	for (bit = bbool.begin(); bit != bbool.end(); ++bit){
		//Base::Console().Message("******1******%d***%s\n",(*bit).bnum,(*bit).str.c_str());
		Savestreamfile << QString::fromStdString((*bit).str);
		QString planestr;
		 int pflag = 0;
		if (QString::fromStdString((*bit).Booleanname).left(5) == QObject::tr("Plane")){//面的布尔运算位移不同
			planestr=QString::fromStdString((*bit).Booleanname).section(QObject::tr(","), 1, 1);
			pflag = 1;
		} 
		string translate;
		if (pflag){
			translate = Booleanoperationinformation(planestr.toStdString());
		} 
		else{
			translate = Booleanoperationinformation((*bit).Booleanname);
		}
		int num;
		string bname = (*bit).Booleanname;
		num = (*bit).bnum;
		for (bits = bbool.begin(); bits != bbool.end();){//判断是否union的最后一步，它可能是多个体进行的
		//Base::Console().Message("******124******%d***%s\n",(*bits).bnum,(*bits).Booleanname.c_str());
			if(bname==(*bits).Booleanname&&num!=(*bits).bnum){
				//Base::Console().Message("******100******%d***%s\n",(*bits).bnum,(*bits).Booleanname.c_str());
				Savestreamfile << QString::fromStdString((*bits).str);//625
				num=(*bits).bnum;
				bits=bbool.erase(bits);
			}else{
				++bits;
			}
		}
		//bitnum++;
		if (translate != "NULL" /*&& num == (*bit).bnum*/){
			//QString tran = QString::fromStdString(translate);
			QString Axsix=QString::fromStdString(translate).section(QObject::tr(","), 0, 0);
			QString Axsiy=QString::fromStdString(translate).section(QObject::tr(","), 1, 1);
			QString Axsiz=QString::fromStdString(translate).section(QObject::tr(","), 2, 2);
			QString angle=QString::fromStdString(translate).section(QObject::tr(","), 3, 3);
			QString x=QString::fromStdString(translate).section(QObject::tr(","), 4, 4);
			QString y=QString::fromStdString(translate).section(QObject::tr(","), 5, 5);
			QString z=QString::fromStdString(translate).section(QObject::tr(","), 6, 6);
			QString Rotation;
			QString Translate;
			if (pflag){
				 Rotation = QObject::tr("Rotate { {%1,%2,%3}, {0,0,0},%4 } { Surface{%5}; }\n").arg(Axsix).arg(Axsiy).arg(Axsiz).arg(angle).arg(num);
				Translate = QObject::tr("Translate {%1,%2,%3}{Surface{%4};}\n").arg(x).arg(y).arg(z).arg(num);
			}
			else{
				Savestreamfile <<QObject::tr("Rotate { {%1,%2,%3}, {0,0,0},%4 } { Volume{%5}; }\n").arg(Axsix).arg(Axsiy).arg(Axsiz).arg(angle).arg(num);
				Savestreamfile << QObject::tr("Translate {%1,%2,%3}{Volume{%4};}\n").arg(x).arg(y).arg(z).arg(num);
			}
			//Savestreamfile << Rotation;
			//Savestreamfile << Translate;
		}
	}


	///////最外层和介质PEC的处理
	int flagRad = 0;
	int CoveringBox = 0;
	string outname = CommonFunction::outObjStrName();//获取最大几何体
	Base::Console().Message("******outname***********************%s\n",outname.c_str());
	QSettings settingrad(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		QString Groups = QString::fromStdString((*it)->getNameInDocument());
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		Base::Console().Message("   name***********************%s\n",(*it)->getNameInDocument());
		int flagRad = CommonFunction::DecideRad(*it);//获取设置的Rad
        Base::Console().Message("   flahRad1***********************%d\n",flagRad);
		if(flag==1){//把设置Rad的标志写入文件
			settingrad.setValue(QObject::tr("%1/RADFLAG").arg(Groups), flagRad);
		}
		if(flag==2){//获取Rad设置的情况，有些面会没有在了，所以要记录最开始设置的情况
			flagRad = settingrad.value(QObject::tr("%1/RADFLAG").arg(Groups)).toInt();
		}
		Base::Console().Message("   flahRad***********************%d\n",flagRad);
		if (flagRad){
			if (QString::fromStdString(outname) == QString(QLatin1String((*it)->getNameInDocument()))){//有最外层和全部面设置为Rad才能为空气罩
				CoveringBox = 1;//获取最外层几何体全部设置了Rad
				break;
			}
		}
	}
	Base::Console().Message("******CoveringBox***********************%d\n",CoveringBox);
	if (outname != "NULL"&&CoveringBox == 0){
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
							QObject::tr("Please set up the external surface as the radiation boundary!"));
		return 0;
	}
	////20184/11
	QSettings settings(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	settings.beginGroup(QObject::tr("model"));
	settings.setValue(QObject::tr("MaxGeom"), QString::fromStdString(outname));
	settings.endGroup();
	list<CommonFunction::geometry>::iterator ito;
	list<CommonFunction::geodelete>::iterator del;
	int flagpec = 0;
	int base = 0;
	int tool = 0;
	int bflag=0;
	int Modelsum = 0;
	int OtherBodyflag=0;//判断是第一次
	int OtherBody=0;
	int firstBody=0;
	QString Fragmentstool=QObject::tr("");//629
    QString FragmenExttool=QObject::tr("");//11-2增加拉伸处理的判断。
	int Fragmentsbase=0;//629
	std::map<int,string> unionbox;
	QStringList boolmap;
	QStringList boollistmap;
    QStringList boolEtmap;//11-2增加拉伸处理的判断。
	QSettings settingsp(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	int msum = settingsp.value(QObject::tr("Model/Modelsum")).toInt();
	double meshsizepart = settingsp.value(QObject::tr("Model/MeshSizeParts")).toDouble();
	QSettings settingpec(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string  cutname = (*it)->getNameInDocument();
		//Base::Console().Message("******2018****627**pec=%d*****CoveringBox=%d****name=%s*************\n",flags,CoveringBox,cutname.c_str());
		QString Groups = QString::fromStdString(cutname);
		int flags = CommonFunction::DecidePec(*it);//获取设置的pec
		if(flag==1){//把设置pec的标志写入文件
			settingpec.setValue(QObject::tr("%1/PECFLAG").arg(Groups), flags);
		}
		if(flag==2){//获取pec设置的情况，有些面会没有在了，所以要记录最开始设置的情况
			flags = settingpec.value(QObject::tr("%1/PECFLAG").arg(Groups)).toInt();
		}
		
		int flagExc = CommonFunction::DecideExci(*it);//获取设置的Exc
		
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		if (cutname == outname){ 
		     int partnum;//626输出只有一个空气罩里面都是平面的网格控制
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				if ((*ito).geom == cutname){
					partnum=(*ito).m;
				}
			}
			for (del = geodelete.begin(); del != geodelete.end(); ++del){
				if ((*del).str == cutname){
					partnum=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
				}
			}
			for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(partnum).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
		   continue;
		}//247
		
		int outflag = 0, cutflag = 0;
        int tensileflag=1;//拉伸出来的平面不能使用剪切函数，所以在这个改了使其使用平面那个函数。
	  if (CoveringBox){
			flagpec = 1;
			int planflags=0;
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				 //if (QString::fromStdString((*ito).geom).left(5) == QObject::tr("Plane")){
                 if (QString::fromStdString((*ito).geom).left(5) == QObject::tr("Plane")){
					 planflags=1;
					continue;
					} 
				if ((*ito).geom == cutname){
					cutflag = 1;
                    tensileflag=1;
					tool = (*ito).m;
                    if(QString::fromStdString((*ito).geom).left(7) == QObject::tr("Extrude")){//11-1
                   // Base::Console().Message("******t***********************\n");
                     tensileflag=0;
                    }
				}
				if (((*ito).geom == outname)&&(bflag==0)){
					outflag = 1;
                    //tensileflag=1;
                    //Base::Console().Message("******ts*******%s****************\n",outname);
					base = (*ito).m;
					Fragmentsbase=(*ito).m;
					for(int i=1;i<=msum;i++){
						QString surface = QString::fromLatin1("Model/Model%1").arg(i);
						QString  bstr = settingsp.value(surface).toString();
						if(bstr.toStdString()==outname){
							Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(base).arg(meshsizepart);
							settingsp.setValue(surface, QObject::tr("NULL"));
						}
					}
				}
			}
          //  Base::Console().Message("******to************%d***********\n",tensileflag);
			for (del = geodelete.begin(); del != geodelete.end(); ++del){//布尔运算重新编号的//布尔运算应该拉伸不会出拉伸，但添加上好像也没用
				//if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//11-1
                    if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//11-1
					planflags=1;
					continue;
				}
				if (((*del).str == outname) && (bflag == 0)){
					outflag = 1;
                    tensileflag=1;
					base=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
					Fragmentsbase=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;//629
					for(int i=1;i<=msum;i++){
						QString surface = QString::fromLatin1("Model/Model%1").arg(i);
						QString  bstr = settingsp.value(surface).toString();
						if(bstr.toStdString()==outname){
							Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(base).arg(meshsizepart);
							settingsp.setValue(surface, QObject::tr("NULL"));
						}
					}
				}
				
				if ((*del).str == cutname){
					//tool = (*del).num1;
					cutflag = 1;
                    tensileflag=1;
					tool = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
                    if(QString::fromStdString((*del).str).left(7) == QObject::tr("Extrude")){//11-1
                   // Base::Console().Message("******ol1***********************\n");
                     tensileflag=0;//拉伸出来的平面不能使用剪切函数，所以在这个改了使其使用平面那个函数。
                     continue;
                    }
				}
			}
		//}
			
			if(flag==2){
				for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(tool).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			}
			//bflag=1;
			QString cut;
            if(tensileflag==0){//拉伸添加11-1
                flags=0;
                //Base::Console().Message("******tool1***********************%d\n",tool);
            }
			if(flags==1){//629修改成使用BooleanFragments的方式,平面的不能加入剪切
				if(tool==0){continue;};
				 cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(base).arg(tool);
				Savestreamfile << cut;
				Fragmentsbase= shapenum;//7-3 
				//Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(tool);
				boolmap<<QObject::tr("%1").arg(tool);
				boolmap<<QObject::tr("%1").arg(base);
				bflag=1;
				base=shapenum;
				//Base::Console().Message("******tool1***********************%d\n",tool);
			}
			else{
				//cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3};};\n").arg(++shapenum).arg(base).arg(tool);
				if(tool==0){continue;};
				if (true == boolmap.contains(QObject::tr("%1").arg(tool))){
					//Base::Console().Message("******tool2***********************%d\n",tool);
					continue;}
				//Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(tool);
				//Base::Console().Message("******tool3***********************%d\n",tool);
                //11-2修改加了一个拉伸的处理
				//boollistmap<<QObject::tr("%1").arg(tool);
                if(tensileflag==0){
                    boolEtmap<<QObject::tr("%1").arg(tool);
                }
                else{
                    boollistmap<<QObject::tr("%1").arg(tool);
                }
			}
			QSettings settingmodel(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
			settingmodel.setValue(QObject::tr("Model/Modelname"), QString::fromStdString(outname));
			settingmodel.setValue(QObject::tr("Model/geoNo"), shapenum);
			settingmodel.setValue(QObject::tr("Model/base"), base);
			settingmodel.setValue(QObject::tr("Model/tool"), tool);

			//base=shapenum;
			

			

		}//if 最外层的处理部分
		else{//不是有空气盒子的合成一体
			int pflag=0;//平面的不进行处理
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				if ((*ito).geom == cutname){
					OtherBody = (*ito).m;
				}
			}
			for (del = geodelete.begin(); del != geodelete.end(); ++del){//布尔运算重新编号的
				if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//面的布尔运算不进行操作
					QString planestr=QString::fromStdString((*del).str).section(QObject::tr(","), 1, 1);
					pflag = 1;
					if (planestr== QString::fromStdString(cutname)){
					OtherBody = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
					}
				} 
				if ((*del).str == cutname){
					OtherBody = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
				}
			}
			/////
			if (flag == 2){
				int msum = settingsp.value(QObject::tr("Model/Modelsum")).toInt();
				double meshsizepart = settingsp.value(QObject::tr("Model/MeshSizeParts")).toDouble();
				for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(OtherBody).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			}
			/////
			if(pflag==1){
				continue;
			}
			unionbox.insert(pair<int, string>(OtherBody, cutname));
		}//else
	}//for
	/////////////2018/912//进行过剪切的就不加入进Fragmentstool里面
	for (int i = 0; i < boollistmap.size(); i++){
		//Base::Console().Message("geolists==%d=%s\n", i, boollistmap[i].toStdString().c_str());
		//int a=boollistmap[i].
		if (true == boolmap.contains(boollistmap[i])){
			//Base::Console().Message("******tool2***********************%d\n",boollistmap[i]);
					continue;
		}
		else{
			//Base::Console().Message("******tool1***********************%d\n",boollistmap[i]);
			Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(boollistmap[i]);
		}
		//Base::Console().Message("******tool***********************%d\n",boollistmap[i]);
	}
    ////11-2增加一个拉伸的判断
    for (int i = 0; i < boolEtmap.size(); i++){
		if (true == boolmap.contains(boolEtmap[i])){
					continue;
		}
		else{
			FragmenExttool=FragmenExttool+QObject::tr("%1,").arg(boolEtmap[i]);
		}
	}
	/////////

	////2018/6-11/////////处理非接触面union的问题
	if (unionbox.size() > 1){
		//Base::Console().Message("******unionbox=%d**********************\n",unionbox.size());
		QString slist;
		string box1, box2;
		int m,n=0;
		for (map<int, string>::iterator iter = unionbox.begin(); iter != unionbox.end(); iter++)
		{
				int i = 0;
				box1 = iter->second;
				m = iter->first;
				for (map<int, string>::iterator ites = unionbox.begin(); ites != unionbox.end(); ites++)
				{
					if (iter->second == ites->second){ continue; }
						box2 = ites->second;
						bool tf = CommonFunction::Booleanunionoperation(box1, box2);
						if (tf){
							i++;
						}
				
				}//ites
				if (i == 1){//只与一个接触可能是开头或者结尾的体
					slist = QString::fromStdString(box1);
					break;
				}
		}//for iter
	
		QStringList mlist;
		QStringList geolists;
		int k = 0;
		App::Document* docc = App::GetApplication().getActiveDocument();
		std::vector<App::DocumentObject*> selsc = docc->getObjectsOfType(App::DocumentObject::getClassTypeId());
		for (std::vector<App::DocumentObject*>::iterator itc = selsc.begin(); itc != selsc.end(); ++itc)
		{
			std::string Group = (*itc)->getNameInDocument();
			QString Groups = QString::fromStdString(Group);
			if (Groups.left(5) == QObject::tr("Group")){
				continue;
			}
			if (Groups.left(5) == QObject::tr("Plane")){
				continue;
			}
			if (Groups.left(4) == QObject::tr("Line")){
				continue;
			}
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*itc);
			bool visibility = true;
			if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			// 读取几何体可见(Visibility)属性值  START  END
			if (visibility == false){// 不可见的跳过
				continue;
			}
			/* if (k == 0)
			{
				slist = Groups;
			} */
			mlist << Groups;
			k++;
		}
		geolists=Sortgeo(slist, k, geolists);
		int geo1;
		int f = 0;
		for (int i = 0; i < geolists.size(); i++){
			for (map<int, string>::iterator nter = unionbox.begin(); nter != unionbox.end();)
			{
				if (geolists[i].toStdString() == nter->second){
					QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
					if (f == 0){ 
						geo1 = nter->first;
						f = 1;
						nter = unionbox.erase(nter);
						break;
					}
					if (f == 1){
						QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(geo1).arg(nter->first);
						Savestreamfile << cut;
						f = 2;
						int i = settingNo.value(QObject::tr("Count/count")).toInt();
						//if（i == NULL{ i = 0; }
						settingNo.setValue(QObject::tr("Count/count"), ++i);
						//Base::Console().Message("200  =%d,flag=%d,shapenum=%d\n",i,flag,shapenum);
						settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
						settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
					}
					else{
						QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(shapenum - 1).arg(nter->first);
						Savestreamfile << cut;
						int i = settingNo.value(QObject::tr("Count/count")).toInt();
						//if（i==NULL{ i = 0; }
						settingNo.setValue(QObject::tr("Count/count"), ++i);
						//Base::Console().Message("202  =%d,flag=%d,shapenum=%d\n",i,flag,shapenum);
						settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
						settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
					}
					nter=unionbox.erase(nter);

				}
				else{
					nter++;
				}
			}//for map
		}//for i
		
		if (unionbox.size()){//可能有些模型没有输出出来。但可能会出现有些模型与其他模型都不接触。
			for (map<int, string>::iterator nter = unionbox.begin(); nter != unionbox.end(); nter++)
			{
				QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(shapenum - 1).arg(nter->first);
				Savestreamfile << cut;
				QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
				int i = settingNo.value(QObject::tr("Count/count")).toInt();
				settingNo.setValue(QObject::tr("Count/count"), ++i);
				//Base::Console().Message("201   =%d,flag=%d\n",i,flag);
			    settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
			    settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
			}
		}

	 
		
	}
	////2018/6-11//end//////
	
	if(Fragmentstool!=QObject::tr("")){
		QString Fragments = QObject::tr("b()=BooleanFragments{ Volume{%1}; Delete; }{Volume{%2}; Delete; };\n").arg(Fragmentsbase).arg(Fragmentstool.left(Fragmentstool.length() - 1));
		Savestreamfile << Fragments;
       
	}
    ////11-2增加一个拉伸的判断
    if(FragmenExttool!=QObject::tr("")){
        if(Fragmentstool!=QObject::tr("")){
            if(ExtrudeDWireface!=QObject::tr("")){//增加DWire形成的面
                 QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{b()}; Delete; }{Surface{%1,%2}; Delete; };\n").arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                 Savestreamfile << Fragments;
            }
            else{
                QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{b()}; Delete; }{Surface{%1}; Delete; };\n").arg(FragmenExttool.left(FragmenExttool.length() - 1));
                Savestreamfile << Fragments;
            }
        }
        else{
             if(ExtrudeDWireface!=QObject::tr("")){//增加DWire形成的面
                 QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{%1}; Delete; }{Surface{%2,%3}; Delete; };\n").arg(Fragmentsbase).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                 Savestreamfile << Fragments;
            }
            else{
                QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; };\n").arg(Fragmentsbase).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                Savestreamfile << Fragments;
            }
        }
	}

	if(Recordplanenum.size()>=1){//二维模块对计算域修改
	   QString planenum=QObject::tr("");
	   QMap<QString,QString>::iterator iter;
		for (iter=Recordplanenum.begin();iter!=Recordplanenum.end();iter++){  
			planenum=planenum+QObject::tr("%1,").arg(iter.key());
			for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr==iter.value()){
					if(flag==2){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Surface{%1};} } = %2;\n").arg(iter.key()).arg(meshsizepart);
						}
					//settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			//Base::Console().Message("201   %s\n", planenum.toStdString().c_str());
		}
		//Base::Console().Message("201 22  %s\n", planenum.toStdString().c_str());
		if(Fragmentstool!=QObject::tr("")){
            if(FragmenExttool!=QObject::tr("")){
                QString Fragments = QObject::tr("BooleanFragments{ Volume{d()}; Delete; }{Surface{%1}; Delete; }\n").arg(planenum.left(planenum.length() - 1));
                Savestreamfile << Fragments;
            }else{
                QString Fragments = QObject::tr("BooleanFragments{ Volume{b()}; Delete; }{Surface{%1}; Delete; }\n").arg(planenum.left(planenum.length() - 1));
                Savestreamfile << Fragments;
            }
		}else{
			if(Fragmentsbase!=0){
				QString Fragments = QObject::tr("BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; }\n").arg(Fragmentsbase).arg(planenum.left(planenum.length() - 1));
				Savestreamfile << Fragments;
			}else{
			QString Fragments = QObject::tr("BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; }\n").arg(shapenum).arg(planenum.left(planenum.length() - 1));
			Savestreamfile << Fragments;
			}
		}
	}
	
	////////initialMesh.ini里面获取2018314//////
	if (flag == 2){
		///////获取geo里面面的编号
		QStringList llist;
		QString SifFilegeo = QString::fromLatin1("../ini/box.geo_unrolled");
		QFile SFilegeo(SifFilegeo);
		if (!SFilegeo.open(QIODevice::ReadOnly))
		{
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr(" read the box.geo_unrolled file failed!"));
			return 1;
		}
		QTextStream SIFstreamfilegeo(&SFilegeo);
		QString bufgeo;
		while (!SIFstreamfilegeo.atEnd())//while start
		{
			bufgeo = SIFstreamfilegeo.readLine();
			/* if (bufgeo.left(12) == QObject::tr("Surface Loop"))//获取构成体的面
			{
				QRegExp seps(QObject::tr("[()]"));
				QString beta = bufgeo.section(seps, 1, 1);
				QRegExp sep(QObject::tr("[{}]"));
				QString b = bufgeo.section(sep, 1, 1);
				llist =llist<<b.split(QObject::tr(","));
			} */
			if (bufgeo.left(9) == QObject::tr("Line Loop"))//获取面的编号信息
		    {
			QRegExp seps(QObject::tr("[()]"));
			QString beta = bufgeo.section(seps, 1, 1);
			llist =llist<<beta;
			
		   }
		}//while end
		//////////geob编号end////////////
		QString fstr = QObject::tr("");
		QString Efstr = QObject::tr("");
		int meshnum = 0;
		QString snums = QObject::tr("");
		//boundary
		QSettings settingsmesh(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
		int bsum = settingsmesh.value(QObject::tr("Boundary/Boundarysum")).toInt();
		if (bsum){
			
			Savestreamfile << QObject::tr("Field[%1] = Attractor;\n").arg(++meshnum);
			for (int i = 1; i <= bsum; i++){
				
				QString surface = QString::fromLatin1("Boundary/Boundary%1").arg(i);
				QString  bstr = settingsmesh.value(surface).toString();
				QSettings Bsettings(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
				int Num = Bsettings.value(QObject::tr("%1/BounCount").arg(bstr)).toInt();
				for (int j = 1; j <= Num; j++){
					
					QString surface = bstr + QString::fromLatin1("/MeshFaceNum%1").arg(j);
					int face = Bsettings.value(surface).toInt();
					
					//fstr = fstr + QObject::tr("%1,").arg(face);
					fstr = fstr + QObject::tr("%1,").arg(llist[face-1]);
				}
			}
			double blcmin = settingsmesh.value(QObject::tr("Boundary/FinestGridTimes")).toDouble();
			double blcmax = settingsmesh.value(QObject::tr("Boundary/CoarestGridTimes")).toDouble();
			double distmin = settingsmesh.value(QObject::tr("Boundary/FinesetDistance")).toDouble();
			double distmax = settingsmesh.value(QObject::tr("Boundary/CoarsetDistance")).toDouble();
			Savestreamfile << QObject::tr("Field[%1].FacesList = { ").arg(meshnum);
			Savestreamfile << fstr.left(fstr.length() - 1);
			Savestreamfile << QObject::tr(" };\n");
			Savestreamfile << QObject::tr("Field[%1] = Threshold;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].IField = %2;\n").arg(meshnum).arg(meshnum-1);
			Savestreamfile << QObject::tr("Field[%1].LcMin = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(blcmin);
			Savestreamfile << QObject::tr("Field[%1].LcMax = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(blcmax);
			Savestreamfile << QObject::tr("Field[%1].DistMin = %2;\n").arg(meshnum).arg(distmin);
			Savestreamfile << QObject::tr("Field[%1].DistMax = %2;\n").arg(meshnum).arg(distmax);
			snums = snums + QObject::tr("%1,").arg(meshnum);
		}
		int esum = settingsmesh.value(QObject::tr("Exciation/Exciationsum")).toInt();
		if (esum){
			Savestreamfile << QObject::tr("Field[%1] = Attractor;\n").arg(++meshnum);
			for (int i = 1; i <= esum; i++){
				QString surface = QString::fromLatin1("Exciation/Exciation%1").arg(i);
				QString  bstr = settingsmesh.value(surface).toString();
				QSettings Esettings(QObject::tr("../ini/Excitation.ini"), QSettings::IniFormat);
				int Num = Esettings.value(QObject::tr("%1/SurfacesNum").arg(bstr)).toInt();
				
				for (int j = 1; j <= Num; j++){
					
					QString surface = bstr + QString::fromLatin1("/MeshFaceNum%1").arg(j);
					int face = Esettings.value(surface).toInt();
					
					Efstr = Efstr + QObject::tr("%1,").arg(llist[face-1]);
					
				}
			}
			double elcmin = settingsmesh.value(QObject::tr("Exciation/FinestGridTimes")).toDouble();
			double elcmax = settingsmesh.value(QObject::tr("Exciation/CoarestGridTimes")).toDouble();
			double eistmin = settingsmesh.value(QObject::tr("Exciation/FinesetDistance")).toDouble();
			double eistmax = settingsmesh.value(QObject::tr("Exciation/CoarsetDistance")).toDouble();
			Savestreamfile << QObject::tr("Field[%1].FacesList = { ").arg(meshnum);
			Savestreamfile << Efstr.left(Efstr.length() - 1);
			Savestreamfile << QObject::tr(" };\n");
			Savestreamfile << QObject::tr("Field[%1] = Threshold;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].IField = %2;\n").arg(meshnum).arg(meshnum-1);
			Savestreamfile << QObject::tr("Field[%1].LcMin = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(elcmin);
			Savestreamfile << QObject::tr("Field[%1].LcMax = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(elcmax);
			Savestreamfile << QObject::tr("Field[%1].DistMin = %2;\n").arg(meshnum).arg(eistmin);
			Savestreamfile << QObject::tr("Field[%1].DistMax = %2;\n").arg(meshnum).arg(eistmax);
			snums = snums + QObject::tr("%1,").arg(meshnum);
		}

		//model

		
		//QString meshfacestr = settingsmesh.value(QObject::tr("Model/facesum")).toString();
		//Base::Console().Message("str=%s||%s\n", fstr.toStdString().c_str(),snums.toStdString().c_str());

		if (snums != QObject::tr("")){
			Savestreamfile << QObject::tr("Field[%1] = Min;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].FieldsList = {%2};\n").arg(meshnum).arg(snums.left(snums.length() - 1));
			Savestreamfile << QObject::tr("Background Field = %1;\n").arg(meshnum);
		}

	}//if
	else{
		int modelMax = 0;
		QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
		list<CommonFunction::geodelete>::iterator ds;
		for (ds = geodelete.begin(); ds != geodelete.end(); ++ds){
			//Base::Console().Message("geodelete==%s,num1ok=%d,num2=%d\n", (*ds).str, (*ds).num1, (*ds).num2);
			if ((*ds).num1 > (*ds).num2){
				modelMax = (*ds).num1;
			}
			else{
				modelMax = (*ds).num2;
			}
			if (QString::fromStdString((*ds).str).left(5) == QObject::tr("Plane")){ continue; }//628平面不写入
			int i = settingNo.value(QObject::tr("Count/count")).toInt();
			settingNo.setValue(QObject::tr("Count/count"), ++i);
			settingNo.setValue(QObject::tr("%1/geoNo").arg(i), modelMax);
			settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString((*ds).str));

		}
		list<CommonFunction::geometry>::iterator ggs;
		for (ggs = geobools.begin(); ggs != geobools.end(); ++ggs){
			//Base::Console().Message("geodbools==%s,num=%d,num2=%d\n", (*ggs).geom, (*ggs).m, (*ggs).num);
			if ((*ggs).m > (*ggs).num){
				modelMax = (*ggs).m;
			}
			else{
				modelMax = (*ggs).num;
			}
			if (QString::fromStdString((*ggs).geom).left(5) == QObject::tr("Plane")){ continue; }//
			int i = settingNo.value(QObject::tr("Count/count")).toInt();
			settingNo.setValue(QObject::tr("Count/count"), ++i);
			settingNo.setValue(QObject::tr("%1/geoNo").arg(i), modelMax);
			settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString((*ggs).geom));
		}
		geopec.clear();
	}
	/////////end2018314/////

	
	SFile.close();
	return 1;
}

