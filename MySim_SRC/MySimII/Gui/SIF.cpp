#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#include <string>
#endif
#include <Base/Reader.h>
#include <Base/Console.h>
#include "SIF.h"

using namespace MySimIIGui;
using namespace std;

void SIF::ui2SIF()
{

//convert information from MySimII-ui.ini to make the MySimII.sif file.
//

//read in ui documents
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
        QString SolverName       = settings.value(QObject::tr("SolverSettings/SolverName")     ).toString();
        QString SolverType       = settings.value(QObject::tr("SolverSettings/SolverType")     ).toString();
        QString WorkDirectory    = settings.value(QObject::tr("SolverSettings/WorkDirectory")  ).toString();
        QString IterativeSolver  = settings.value(QObject::tr("SolverSettings/IterativeSolver")).toString();
        QString DirectiveSolver  = settings.value(QObject::tr("SolverSettings/DirectiveSolver")).toString();
        QString Starting         = settings.value(QObject::tr("SolverSettings/Starting")       ).toString();
        QString Ending           = settings.value(QObject::tr("SolverSettings/Ending")         ).toString();
        QString StepSize         = settings.value(QObject::tr("SolverSettings/StepSize")       ).toString();
        QString ConvergRelatErr  = settings.value(QObject::tr("SolverSettings/ConvergRelatErr")).toString();
        QString OutputSteps      = settings.value(QObject::tr("SolverSettings/OutputSteps")    ).toString();
        QString MaxIterSteps     = settings.value(QObject::tr("SolverSettings/MaxIterSteps")   ).toString();
        QString Scaling          = settings.value(QObject::tr("SolverSettings/Scaling")        ).toString();

	if(WorkDirectory.isEmpty()) WorkDirectory = QObject::tr("../ini/");
        
//maping kewords
	QMap<QString, QString> ui2sif;
	ui2sif[QObject::tr("<<SolverName>>")]        = SolverName;
	ui2sif[QObject::tr("<<SolverType>>")]        = SolverType;
	ui2sif[QObject::tr("<<WorkDirectory>>")]     = WorkDirectory;
	ui2sif[QObject::tr("<<Starting>>")]          = Starting;
	ui2sif[QObject::tr("<<Ending>>")]            = Ending;
	ui2sif[QObject::tr("<<StepSize>>")]          = StepSize;
	QString TimeStepIntervals;
	if(StepSize.isEmpty()||abs(StepSize.toDouble())==0.0)
	{ 
	  TimeStepIntervals.sprintf("%.0f", 1);
	}
	else
        {
	  TimeStepIntervals.sprintf("%.0f", 1 + (Ending.toDouble() - Starting.toDouble())/StepSize.toDouble());
	}
	ui2sif[QObject::tr("<<StepIntervals>>")]     = TimeStepIntervals;
	ui2sif[QObject::tr("<<ConvergRelatErr>>")]   = ConvergRelatErr;
	ui2sif[QObject::tr("<<OutputSteps>>")]       = OutputSteps;
	ui2sif[QObject::tr("<<MaxIterSteps>>")]      = MaxIterSteps;
	ui2sif[QObject::tr("<<Scaling>>")]           = Scaling;
	if( IterativeSolver == QObject::tr("TRUE") ) {
	      ui2sif[QObject::tr("<<Iter_YN>>")]   = QObject::tr("Iterative");
        }
	else {
	      ui2sif[QObject::tr("<<Iter_YN>>")]   = QObject::tr("Directive");
	}

	QString Filename   = QObject::tr("../UserDir/prototype%1.sif").arg(SolverName);
	QString Outputname = QObject::tr("../ini/MySimII.sif");
        
//fill the first part of prototype ../UserDir/prototype*.sif
	QFile SIF_proto(Filename);
        if( !SIF_proto.open(QIODevice::ReadOnly) || !SIF_proto.exists() ) {
	    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("\nErr: missing prototype file "), Filename);
	}
        QTextStream proto_stream(&SIF_proto);

	QFile SIF_final(Outputname);
	if(SIF_final.exists()) SIF_final.remove();
        if( !SIF_final.open(QFile::WriteOnly) ) {
	    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("\nErr: Failed to create file "), Outputname);
	}
        QTextStream final_stream(&SIF_final);

        QString line;
        while( ! proto_stream.atEnd() ) {
           line = proto_stream.readLine();
	   bool dump = true;
           for(QMap<QString, QString>::iterator iter = ui2sif.begin(); iter != ui2sif.end(); ++iter) {
	       QString check = line ;
	       line.replace(iter.key(), iter.value());
	       if( QString::compare(check,line) != 0 && iter.value().isEmpty() ) dump = false;
	   }
	   if( dump ) final_stream << QObject::tr("\n%1").arg(line);
        }

        final_stream << QObject::tr("!-----------------------------------------------------\n");
	final_stream << QObject::tr("!  For all boundary settings                          \n");
	final_stream << QObject::tr("!-----------------------------------------------------\n");
        
	SIF_proto.close();

//for the boundaries
	int count_setting = settings.value(QObject::tr("BSettingHead/Count")).toInt();
	if( count_setting == 0 ) {
	   QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			QObject::tr(" No boundary settings !"));
	}
        for(int ib = 1; ib < count_setting+1; ib++) {
	   
           QString ibs;
	   ibs.setNum(ib);
	   ui2sif[QObject::tr("<<ibs>>")]            = ibs;
	   
// creating a temporary file ../ini/proto_tmp.sif to record the current BSettings
	   QString tFile = QObject::tr("../ini/proto_tmp.sif");
	   QFile proto_tmp(tFile);
	   if(proto_tmp.exists()) proto_tmp.remove();
           if( !proto_tmp.open(QIODevice::WriteOnly) ) {
	       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("\nErr: Failed to create "), tFile);
	   }
           QTextStream tFileRecord(&proto_tmp);

// gathering information of the current BSettings
	   QString BCount        = settings.value(QObject::tr("BSetting%1/count"        ).arg(ibs)).toString();
	   QString BFaces        = settings.value(QObject::tr("BSetting%1/BFaces"       ).arg(ibs)).toString();
	   QString SelfDef1      = settings.value(QObject::tr("BSetting%1/SelfDef1"     ).arg(ibs)).toString();
	   QString SelfDef2      = settings.value(QObject::tr("BSetting%1/SelfDef2"     ).arg(ibs)).toString();
	   QString CF_LCoord     = settings.value(QObject::tr("BSetting%1/CF_LCoord"    ).arg(ibs)).toString();
	   QString CF_Wall       = settings.value(QObject::tr("BSetting%1/CF_Wall"      ).arg(ibs)).toString();
	   QString CF_FreeSurf   = settings.value(QObject::tr("BSetting%1/CF_FreeSurf"  ).arg(ibs)).toString();
	   QString CF_FreeMoving = settings.value(QObject::tr("BSetting%1/CF_FreeMoving").arg(ibs)).toString();
	   QString CF_Vel1       = settings.value(QObject::tr("BSetting%1/CF_Vel1"      ).arg(ibs)).toString();
	   QString CF_Vel2       = settings.value(QObject::tr("BSetting%1/CF_Vel2"      ).arg(ibs)).toString();
	   QString CF_Vel3       = settings.value(QObject::tr("BSetting%1/CF_Vel3"      ).arg(ibs)).toString();
	   QString CF_Pres       = settings.value(QObject::tr("BSetting%1/CF_Pres"      ).arg(ibs)).toString();
	   QString CF_SlipCoeff  = settings.value(QObject::tr("BSetting%1/CF_SlipCoeff" ).arg(ibs)).toString();
	   QString Heat_Temp     = settings.value(QObject::tr("BSetting%1/Heat_Temp"    ).arg(ibs)).toString();
	   QString Heat_Flux     = settings.value(QObject::tr("BSetting%1/Heat_Flux"    ).arg(ibs)).toString();
	   QString EM_Type       = settings.value(QObject::tr("BSetting%1/EM_Type"      ).arg(ibs)).toString();
	   QString IncWaveX      = settings.value(QObject::tr("BSetting%1/IncWaveX"     ).arg(ibs)).toString();
	   QString IncWaveY      = settings.value(QObject::tr("BSetting%1/IncWaveY"     ).arg(ibs)).toString();
	   QString IncWaveZ      = settings.value(QObject::tr("BSetting%1/IncWaveZ"     ).arg(ibs)).toString();
	   QString IncPropX      = settings.value(QObject::tr("BSetting%1/IncPropX"     ).arg(ibs)).toString();
	   QString IncPropY      = settings.value(QObject::tr("BSetting%1/IncPropY"     ).arg(ibs)).toString();
	   QString IncPropZ      = settings.value(QObject::tr("BSetting%1/IncPropZ"     ).arg(ibs)).toString();
	   
	   ui2sif[QObject::tr("<<BCount>>")]            = BCount;
	   ui2sif[QObject::tr("<<BFaces>>")]            = BFaces;
	   ui2sif[QObject::tr("<<SelfDef1>>")]          = SelfDef1;
	   ui2sif[QObject::tr("<<SelfDef2>>")]          = SelfDef2;
	   ui2sif[QObject::tr("<<CF_LCoord>>")]         = CF_LCoord;
	   ui2sif[QObject::tr("<<CF_Wall>>")]           = CF_Wall;
	   ui2sif[QObject::tr("<<CF_FreeSurf>>")]       = CF_FreeSurf;
	   ui2sif[QObject::tr("<<CF_FreeMoving>>")]     = CF_FreeMoving;
	   ui2sif[QObject::tr("<<CF_Vel1>>")]           = CF_Vel1;
	   ui2sif[QObject::tr("<<CF_Vel2>>")]           = CF_Vel2;
	   ui2sif[QObject::tr("<<CF_Vel3>>")]           = CF_Vel3;
	   if( CF_Vel1.isEmpty() ) {
	         ui2sif[QObject::tr("<<CF_SlipCoeff1>>")]   = CF_SlipCoeff;
           }
	   else {
	         ui2sif[QObject::tr("<<CF_SlipCoeff1>>")]   = QObject::tr("");
	   }
	   if( CF_Vel2.isEmpty() ) {
	         ui2sif[QObject::tr("<<CF_SlipCoeff2>>")]   = CF_SlipCoeff;
           }
	   else {
	         ui2sif[QObject::tr("<<CF_SlipCoeff2>>")]   = QObject::tr("");
	   }
	   if( CF_Vel3.isEmpty() ) {
	         ui2sif[QObject::tr("<<CF_SlipCoeff3>>")]   = CF_SlipCoeff;
           }
	   else {
	         ui2sif[QObject::tr("<<CF_SlipCoeff3>>")]   = QObject::tr("");
	   }
	   ui2sif[QObject::tr("<<CF_Pres>>")]           = CF_Pres;
	   ui2sif[QObject::tr("<<Heat_Temp>>")]         = Heat_Temp;
	   ui2sif[QObject::tr("<<Heat_Flux>>")]         = Heat_Flux;
	   if( Heat_Flux.isEmpty() ) {
	         ui2sif[QObject::tr("<<Heat_F_YN>>")]   = QObject::tr("False");
           }
	   else {
	         ui2sif[QObject::tr("<<Heat_F_YN>>")]   = QObject::tr("True");
	   }
	   ui2sif[QObject::tr("<<EM_Type>>")]           = EM_Type;
	   ui2sif[QObject::tr("<<IncWaveX>>")]          = IncWaveX;
	   ui2sif[QObject::tr("<<IncWaveY>>")]          = IncWaveY;
	   ui2sif[QObject::tr("<<IncWaveZ>>")]          = IncWaveZ;
	   ui2sif[QObject::tr("<<IncPropX>>")]          = IncPropX;
	   ui2sif[QObject::tr("<<IncPropY>>")]          = IncPropY;
	   ui2sif[QObject::tr("<<IncPropZ>>")]          = IncPropZ;
	  
           QString BType    = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("BType=")   )+6,  4 );
           QString LCoordX  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("LCoordX=") )+8, 14 );
           QString LCoordY  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("LCoordY=") )+8, 14 );
           QString LCoordZ  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("LCoordZ=") )+8, 14 );
           QString ONormalX = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("ONormalX="))+9, 14 );
           QString ONormalY = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("ONormalY="))+9, 14 );
           QString ONormalZ = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("ONormalZ="))+9, 14 );
           QString Edge1X   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge1X=")  )+7, 14 );
           QString Edge1Y   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge1Y=")  )+7, 14 );
           QString Edge1Z   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge1Z=")  )+7, 14 );
           QString Edge2X   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge2X=")  )+7, 14 );
           QString Edge2Y   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge2Y=")  )+7, 14 );
           QString Edge2Z   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Edge2Z=")  )+7, 14 );
           QString Length1  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Length1=") )+8, 14 );
           QString Length2  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Length2=") )+8, 14 );
           QString Radius   = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Radius=")  )+7, 14 );
           QString Radius1  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Radius1=") )+8, 14 );
           QString Radius2  = SelfDef1.mid( SelfDef1.indexOf(QObject::tr("Radius2=") )+8, 14 );
	   ui2sif[QObject::tr("<<LCoordX>>")]           = LCoordX;
	   ui2sif[QObject::tr("<<LCoordY>>")]           = LCoordY;
	   ui2sif[QObject::tr("<<LCoordZ>>")]           = LCoordZ;
	   ui2sif[QObject::tr("<<ONormalX>>")]          = ONormalX;
	   ui2sif[QObject::tr("<<ONormalY>>")]          = ONormalY;
	   ui2sif[QObject::tr("<<ONormalZ>>")]          = ONormalZ;
	   ui2sif[QObject::tr("<<Edge1X>>")]            = Edge1X;
	   ui2sif[QObject::tr("<<Edge1Y>>")]            = Edge1Y;
	   ui2sif[QObject::tr("<<Edge1Z>>")]            = Edge1Z;
	   ui2sif[QObject::tr("<<Edge2X>>")]            = Edge2X;
	   ui2sif[QObject::tr("<<Edge2Y>>")]            = Edge2Y;
	   ui2sif[QObject::tr("<<Edge2Z>>")]            = Edge2Z;
	   ui2sif[QObject::tr("<<Length1>>")]           = Length1;
	   ui2sif[QObject::tr("<<Length2>>")]           = Length2;
	   ui2sif[QObject::tr("<<Radius>>")]            = Radius;
	   ui2sif[QObject::tr("<<Radius1>>")]           = Radius1;
	   ui2sif[QObject::tr("<<Radius2>>")]           = Radius2;


	   tFileRecord << QObject::tr("                                                      \n");
	   tFileRecord << QObject::tr("!-----------------------------------------------------\n");
	   tFileRecord << QObject::tr("!  the boundary settings # <<ibs>>                    \n");
	   tFileRecord << QObject::tr("!-----------------------------------------------------\n");
//
//
//
//
//
//  definging functions prior to boundaries
//
//
//
//
//
           if(SolverName.indexOf(QObject::tr("EM"  )) >= 0 ) {
              if(EM_Type.indexOf(QObject::tr("WavePort")) >= 0 ) {
                if( BType.indexOf(QObject::tr("Rect")) >= 0 ) {
	          tFileRecord << QObject::tr("                                               \n"
"$ function gfun<<ibs>>(c,omg,beta) { mu_r=pi*4e-7;\\                                        \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      nxx=<<Edge1X>>; nxy=<<Edge1Y>>; nxz=<<Edge1Z>>; \\                    \n"
"                      W=<<Length1>>;H=<<Length2>>;                    \\                    \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      dx=ny*nxz-nz*nxy; \\                                                  \n"
"                      dy=nz*nxx-nx*nxz; \\                                                  \n"
"                      dz=nx*nxy-ny*nxx; \\                                                  \n"
"                      x=dx*(c(1)-xr)+dy*(c(2)-yr)+dz*(c(3)-zr); \\                          \n"
"                      if(beta==0) E0=0; \\                                                  \n"
"                      else E0=2*sqrt(omg*mu_r/abs(beta)/H/W)*cos(x*pi/W); \\                \n"
"                      ex=nxx*E0; \\                                                         \n"
"                      ey=nxy*E0; \\                                                         \n"
"                      ez=nxz*E0; \\                                                         \n"
"                      gcomp(0)=(ny*ny+nz*nz)*ex-nx*ny*ey-nx*nz*ez; \\                       \n"
"                      gcomp(1)=-nx*ny*ex+(nx*nx+nz*nz)*ey-ny*nz*ez; \\                      \n"
"                      gcomp(2)=-nx*nz*ex-ny*nz*ey+(nx*nx+ny*ny)*ez; \\                      \n"
"                     _gfun<<ibs>>=gcomp; }                                                  \n");
		}
                if( BType.indexOf(QObject::tr("Circ")) >= 0 ) {
		  
		}
                if( BType.indexOf(QObject::tr("Ring")) >= 0 ) {
	          tFileRecord << QObject::tr("                                               \n"
"$ function gfun<<ibs>>(c,omg,beta) {              \\                                        \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      x=c(1)-xr;y=c(2)-yr;z=c(3)-zr; \\                                     \n"
"                      eta=1.0/sqrt(8.854e-12/1.2566e-6); \\                                 \n"
"                      r1=<<adius1>>; r2=<<adius2>>; \\                                      \n"
"                      Er=sqrt(ln(r2/r1)/eta/2.0/pi); \\                                     \n"
"                      r=(x*x+y*y+z*z)^0.5;E0=Er/r/r/ln(r2/r1); \\                           \n"
"                      ex=x*E0;   \\                                                         \n"
"                      ey=y*E0;   \\                                                         \n"
"                      ez=z*E0;   \\                                                         \n"
"                      gcomp(0)=(ny*ny+nz*nz)*ex-nx*ny*ey-nx*nz*ez; \\                       \n"
"                      gcomp(1)=-nx*ny*ex+(nx*nx+nz*nz)*ey-ny*nz*ez; \\                      \n"
"                      gcomp(2)=-nx*nz*ex-ny*nz*ey+(nx*nx+ny*ny)*ez; \\                      \n"
"                     _gfun<<ibs>>=gcomp; }                                                  \n");
		  
		}

	        tFileRecord << QObject::tr("                                                 \n"
"$ function phase<<ibs>>(c,b) {                                      \\                      \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      x=c(1)-xr;y=c(2)-yr;z=c(3)-zr; \\                                     \n"
"                      bx=nx;by=ny;bz=nz;distance=x*bx+y*by+z*bz;  \\                        \n"
"                      if(b>0) fcomp(0)=cos(b*distance); else fcomp(0)=-exp(b*distance);\\   \n"
"                      if(b>0) fcomp(1)=sin(b*distance); else fcomp(1)=0.0;      \\          \n"
"                     _phase<<ibs>>=fcomp }                                                  \n");
	      } 
              if(EM_Type.indexOf(QObject::tr("LumpPort")) >= 0 ) {
                if( BType.indexOf(QObject::tr("Rect")) >= 0 ) {
	          tFileRecord << QObject::tr("                                               \n"
"$ function gfun<<ibs>>(c,omg,beta) { mu_r=pi*4e-7;\\                                        \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      nxx=<<Edge1X>>; nxy=<<Edge1Y>>; nxz=<<Edge1Z>>; \\                    \n"
"                      W=<<Length1>>;H=<<Length2>>;                    \\                    \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      E0=15.0/H; \\                                                         \n"
"                      ex=nxx*E0; \\                                                         \n"
"                      ey=nxy*E0; \\                                                         \n"
"                      ez=nxz*E0; \\                                                         \n"
"                      gcomp(0)=(ny*ny+nz*nz)*ex-nx*ny*ey-nx*nz*ez; \\                       \n"
"                      gcomp(1)=-nx*ny*ex+(nx*nx+nz*nz)*ey-ny*nz*ez; \\                      \n"
"                      gcomp(2)=-nx*nz*ex-ny*nz*ey+(nx*nx+ny*ny)*ez; \\                      \n"
"                     _gfun<<ibs>>=gcomp; }                                                  \n");
		}
                if( BType.indexOf(QObject::tr("Circ")) >= 0 ) {
		  
		}
                if( BType.indexOf(QObject::tr("Ring")) >= 0 ) {
	          tFileRecord << QObject::tr("                                               \n"
"$ function gfun<<ibs>>(c,omg,beta) {              \\                                        \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      x=c(1)-xr;y=c(2)-yr;z=c(3)-zr; \\                                     \n"
"                      r1=<<adius1>>; r2=<<adius2>>; \\                                      \n"
"                      r=(x*x+y*y+z*z)^0.5;E0=15.0/r/r/ln(r2/r1); \\                         \n"
"                      ex=x*E0;   \\                                                         \n"
"                      ey=y*E0;   \\                                                         \n"
"                      ez=z*E0;   \\                                                         \n"
"                      gcomp(0)=(ny*ny+nz*nz)*ex-nx*ny*ey-nx*nz*ez; \\                       \n"
"                      gcomp(1)=-nx*ny*ex+(nx*nx+nz*nz)*ey-ny*nz*ez; \\                      \n"
"                      gcomp(2)=-nx*nz*ex-ny*nz*ey+(nx*nx+ny*ny)*ez; \\                      \n"
"                     _gfun<<ibs>>=gcomp; }                                                  \n");
		  
		}

	        tFileRecord << QObject::tr("                                                 \n"
"$ function phase<<ibs>>(c,b) {                                      \\                      \n"
"                      xr=<<LCoordX>>;yr=<<LCoordY>>;zr=<<LCoordZ>>; \\                      \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      x=c(1)-xr;y=c(2)-yr;z=c(3)-zr; \\                                     \n"
"                      bx=nx;by=ny;bz=nz;distance=x*bx+y*by+z*bz;  \\                        \n"
"                      if(b>0) fcomp(0)=cos(b*distance); else fcomp(0)=-exp(b*distance);\\   \n"
"                      if(b>0) fcomp(1)=sin(b*distance); else fcomp(1)=0.0;      \\          \n"
"                     _phase<<ibs>>=fcomp }                                                  \n");

	      } 
              if(EM_Type.indexOf(QObject::tr("Inc.Wave")) >= 0 ) {
	        tFileRecord << QObject::tr("                                                 \n"
"$ function En_fun<<ibs>>(c) { \\                                                            \n"
"                      nx=<<ONormalX>>; ny=<<ONormalY>>; nz=<<ONormalZ>>; \\                 \n"
"                      ex=<<IncWaveX>>; ey=<<IncWaveY>>; ez=<<IncWaveZ>>; \\                 \n"
"                      en=nx*ex+ny*ey+nz*ez;                              \\                 \n"
"                      e(0)=ex-en*nx; \\                                                     \n"
"                      e(1)=ey-en*ny; \\                                                     \n"
"                      e(2)=ez-en*nz; \\                                                     \n"
"                     _En_fun<<ibs>>=-e; }                                                   \n"
"$ function phase<<ibs>>(c,b) {x=c(1);y=c(2);z=c(3);  \\                                     \n"
"                         bx=<<IncPropX>>;by=<<IncPropY>>;bz=<<IncPropZ>>;  \\               \n"
"                         distance=x*bx+y*by+z*bz;  \\                                       \n"
"                         if(b>0) fcomp(0)=cos(b*distance); \\                               \n" 
"                         else fcomp(0)=-exp(b*distance);\\                                  \n"
"                         if(b>0) fcomp(1)=sin(b*distance); else fcomp(1)=0.0;      \\       \n"
"                        _phase<<ibs>>=fcomp }                                               \n");
	      } 
	   }
//
//
//
//
//
// writing the main body to ../ini/proto_tmp.sif 
//
//
//
//
//
	   tFileRecord << QObject::tr("                                                                            \n");
	   tFileRecord << QObject::tr("Boundary Condition <<ibs>>                                                  \n");
	   tFileRecord << QObject::tr("  Target Boundaries(<<BCount>>) = <<BFaces>>                                \n");
	   tFileRecord << QObject::tr("  Name = BCName<<ibs>>                                                      \n");
           if(SolverName.indexOf(QObject::tr("EM"  )) >= 0 ) {
              if(EM_Type.indexOf(QObject::tr("PEC"     )) >= 0 ) {
	   tFileRecord << QObject::tr("  E re {e} = 0                                                              \n");
	   tFileRecord << QObject::tr("  E im {e} = 0                                                              \n");
	      } 
              if(EM_Type.indexOf(QObject::tr("Rad."    )) >= 0 ) {
	   tFileRecord << QObject::tr("  Electric Robin Coefficient im = Variable time                             \n");
	   tFileRecord << QObject::tr("  Real MATC \"fr(tx)*g1\"                                                   \n");
	      } 
              if(EM_Type.indexOf(QObject::tr("WavePort")) >= 0 ) {
                if( BType.indexOf(QObject::tr("Rect")) >= 0 ) {
	          tFileRecord << QObject::tr("                                                                             \n"
"  Magnetic Boundary Load 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); -2*b*fv(1)*gv(0)\"   \n"
"  Magnetic Boundary Load 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); -2*b*fv(1)*gv(1)\"   \n"
"  Magnetic Boundary Load 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); -2*b*fv(1)*gv(2)\"   \n"
"  Magnetic Boundary Load im 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); 2*b*fv(0)*gv(0)\"    \n"
"  Magnetic Boundary Load im 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); 2*b*fv(0)*gv(1)\"    \n"
"  Magnetic Boundary Load im 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=beta(k,<<Length2>>);gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); 2*b*fv(0)*gv(2)\"    \n"
"  Electric Robin Coefficient = Variable time                                                                              \n"
"  Real MATC \"k=fr(tx)*g1;b=beta(k,<<Length2>>);beta0_re(b)\"                                                             \n"
"  Electric Robin Coefficient im = Variable time                                                                           \n"
"  Real MATC \"k=fr(tx)*g1;b=beta(k,<<Length2>>);beta0_im(b)\"                                                             \n");
		}
                if( BType.indexOf(QObject::tr("Circ")) >= 0 ) {
		  
		}
                if( BType.indexOf(QObject::tr("Ring")) >= 0 ) {
	          tFileRecord << QObject::tr("                                                                             \n"
"  Magnetic Boundary Load 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); -2*b*fv(1)*gv(0)\"                             \n"
"  Magnetic Boundary Load 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); -2*b*fv(1)*gv(1)\"                             \n"
"  Magnetic Boundary Load 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                      \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); -2*b*fv(1)*gv(2)\"                             \n"
"  Magnetic Boundary Load im 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); 2*b*fv(0)*gv(0)\"                              \n"
"  Magnetic Boundary Load im 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); 2*b*fv(0)*gv(1)\"                              \n"
"  Magnetic Boundary Load im 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                   \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<~>(tx,f,b);fv=phase<~>(tx,b); 2*b*fv(0)*gv(2)\"                              \n"
"  Electric Robin Coefficient im = Variable time                                                                           \n"
"  Real MATC \"fr(tx)*g1\"                                                                                                 \n");
		}
	      } 
              if(EM_Type.indexOf(QObject::tr("LumpPort")) >= 0 ) {
	          tFileRecord << QObject::tr("                                                                             \n"
"  E re {e} 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(0)*gv(0)\"                          \n"
"  E re {e} 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(0)*gv(1)\"                          \n"
"  E re {e} 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(0)*gv(2)\"                          \n"
"  E im {e} 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(1)*gv(0)\"                          \n"
"  E im {e} 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(1)*gv(1)\"                          \n"
"  E im {e} 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;b=k;gv=gfun<<ibs>>(tx,f,b);fv=phase<<ibs>>(tx,b); fv(1)*gv(2)\"                          \n"
"  Electric Robin Coefficient im = Variable time                                                                           \n"
"  Real MATC \"fr(tx)*g1\"                                                                                                 \n");
	      } 
              if(EM_Type.indexOf(QObject::tr("Inc.Wave")) >= 0 ) {
	          tFileRecord << QObject::tr("                                                                             \n"
"  E re {e} 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(0)*fv(0)\"                                \n"
"  E re {e} 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(1)*fv(0)\"                                \n"
"  E re {e} 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(2)*fv(0)\"                                \n"
"  E im {e} 1 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(0)*fv(1)\"                                \n"
"  E im {e} 2 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(1)*fv(1)\"                                \n"
"  E im {e} 3 = Variable time, coordinate 1, coordinate 2, coordinate 3                                                    \n"
"  Real MATC \"f=fr(tx(0));k=f*g1;En=En_fun<<ibs>>(tx);fv=phase<<ibs>>(tx,k); En(2)*fv(1)\"                                \n");
	      } 
	   }

           if(SolverName.indexOf(QObject::tr("CF"  )) >= 0 ) {
	        tFileRecord << QObject::tr("                                                 \n"
"  Free Surface = Logical <<CF_FreeSurf>>                                                    \n"
"  Free Moving  = Logical <<CF_FreeMoving>>                                                  \n"
"  Wall Law     = Logical <<CF_Wall>>                                                        \n"
"  Normal-Tangential Velocity = Logical <<CF_LCoord>>                                        \n"
"  Calculate Fluidic Force     = Logical True                                                \n"
"  Velocity 1 = Real <<CF_Vel1>>                                                             \n"
"  Velocity 2 = Real <<CF_Vel2>>                                                             \n"
"  Velocity 3 = Real <<CF_Vel3>>                                                             \n"
"  Slip Coefficient 1 = Real <<CF_SlipCoeff1>>                                               \n"
"  Slip Coefficient 2 = Real <<CF_SlipCoeff2>>                                               \n"
"  Slip Coefficient 3 = Real <<CF_SlipCoeff3>>                                               \n"
"  External Pressure = Real <<CF_Pres>>                                                      \n");
	   }

           if(SolverName.indexOf(QObject::tr("Heat")) >= 0 ) {
	        tFileRecord << QObject::tr("                                                 \n"
"  Temperature  = Real <<Heat_Temp>>                                                         \n"
"  Heat Flux BC = Logical <<Heat_F_YN>>                                                      \n"
"  Heat Flux    = Real <<Heat_Flux>>                                                         \n");
	   }

	   tFileRecord << QObject::tr("End                                                   \n");
	   proto_tmp.close(); //closing the file ../ini/proto_tmp.sif

// writing the main body to finale_stream = ../ini/MySimII.sif 
	   QString sFile = QObject::tr("../ini/proto_tmp.sif");
	   QFile proto_tmp_s(sFile); 
           if( !proto_tmp_s.open(QIODevice::ReadOnly)||!proto_tmp_s.exists() ) {
	       QMessageBox::warning(Gui::getMainWindow(), QObject::tr("\nErr: Failed to read "), sFile);
	   }
           QTextStream sFileRead(&proto_tmp_s);
           while( ! sFileRead.atEnd() ) {
              line = sFileRead.readLine();
	      bool dump = true;
              for(QMap<QString, QString>::iterator iter = ui2sif.begin(); iter != ui2sif.end(); ++iter) {
	          QString check = line ;
		  line.replace(iter.key(), iter.value());
		  if( QString::compare(check,line) != 0 && iter.value().isEmpty() ) dump = false;
	      }
	      if( dump ) final_stream << QObject::tr("\n%1").arg(line); 
           }
	   proto_tmp_s.close(); //closing the file ../ini/proto_tmp_s.sif

	} // looping BSettings


        SIF_final.close(); //closing the file ../ini/MySimII.sif

	Base::Console().Message("\nui2SIF PASSED\n");

}


