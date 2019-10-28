// ===============================
//      Convert .ply to .spbr
// ===============================

#include <iostream>
#include <cstring> 
#include <cstdlib>
#include <vector>
#include "importPointClouds.h"
#include "writeSPBR.h"

#include <kvs/PolygonObject>
#include <kvs/PointObject>
#include <kvs/glut/Application> 
#include <kvs/glut/Screen>
#include <kvs/Camera>
#include <kvs/PointRenderer> 
#include <kvs/Coordinate> 

#define DEBUG_SELECT_AREA
#define EXEC_VIA_SPBR
//#define EXEC_VIA_KVS

const char OUT_FILE[] = "SPBR_DATA/out.spbr";

int main( int argc, char** argv ) {
    char outSPBRfile[512];
    strcpy( outSPBRfile, OUT_FILE ); 

    if ( argc < 9 ) {
        std::cout   << "\n----- USAGE -----\n" << argv[0] << " [input_file] [output_file] x_min y_min z_min x_max y_max z_max\n"
                    << "\n----- For example -----\n"
                    << "$ " << argv[0] << " input.ply output.spbr\n" << std::endl;
        exit(1);

    } else {
        strcpy( outSPBRfile, argv[2] );
    }
    
    // ----- Import "point cloud data（.ply, argv[1]）" that user selected
    // ----- Inheritance of KVS::PolygonObject -----
    ImportPointClouds *ply = new ImportPointClouds( argv[1] );
    ply->updateMinMaxCoords();
    std::cout << "PLY Min, Max Coords:" << std::endl;
    std::cout << "Min : " << ply->minObjectCoord() << std::endl;
    std::cout << "Max : " << ply->maxObjectCoord() << std::endl;

    //ply->setColors( kvs::ValueArray<kvs::UInt8>( colors ) );

    // ----- Write .spbr file -----
    // set
    float select_area_min_max[6]={0};
    for (int i = 0; i < 6; i++) {
        select_area_min_max[i] = atof(argv[3+i]);
    }
    
    
    WritingDataType type = Ascii;
    writeSPBR(  ply,                    /* kvs::PolygonObject *_ply        */  
                outSPBRfile,            /* char*              _filename    */  
                select_area_min_max,   /* float              _select_area_min_max[] */
                type                    /* WritingDataType    _type        */
                );       

    // ----- Convert "PolygonObject（KVS）" to "PointObject（KVS）" -----
    kvs::PointObject* object = new kvs::PointObject( *ply );
    object->setSize( 1 );
    object->updateMinMaxCoords(); 



    // ----- Exec. SPBR -----
#ifdef EXEC_VIA_SPBR
    std::string out_noised_spbr( outSPBRfile );
    std::string EXEC("spbr ");
    EXEC += out_noised_spbr;
    system( EXEC.c_str() );

    return 0;
#endif

    // ----- Exec. KVS -----
#ifdef EXEC_VIA_KVS
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    kvs::PointRenderer* renderer = new kvs::PointRenderer(); 
    renderer->enableTwoSideLighting(); 
    screen.setTitle( "Point Object" );
    
    kvs::Vector3f cam_pos(0, 8, 0);
    kvs::Vector3f cam_up(0, 0, 1);
    
    screen.setBackgroundColor( kvs::RGBColor(0, 0, 0) );
    //screen.setBackgroundColor( kvs::RGBColor(255, 255, 255) );
    screen.setGeometry(0, 0, 1000, 1000);
    screen.scene()->camera()->setPosition(cam_pos);
    screen.scene()->camera()->setUpVector(cam_up);
    screen.registerObject(object, renderer);
    screen.show();

    return app.run();
#endif
}
