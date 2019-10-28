#ifndef _writeSPBR_H__
#define _writeSPBR_H__

#include <kvs/PolygonObject>
#include <vector>
#include <fstream>
#include <cstdlib>

enum WritingDataType {
    Ascii = 0,
    Binary = 1
};

// default
const float NORMAL[3]   = { 0.0, 0.0, 0.0 };
const int   COLOR[3]    = { 255, 255, 255 };

/*===========================================================================*/
/**
 *  @brief  Write noise intensity to outfile.
 *  @param  *ply        point cloud
 *  @param  filename    output file name
 *  @param  type        Ascii or Binary
 **/
/*===========================================================================*/
void writeSPBR( kvs::PolygonObject *_ply, 
                char* _filename,  
                float _select_area_min_max[],
                WritingDataType _type = Ascii) {
    size_t  num         = _ply->numberOfVertices(); 
    bool    hasNormal   = false;
    bool    hasColor    = false;
    if( num == _ply->numberOfNormals() ) hasNormal   = true; 
    if( num == _ply->numberOfColors() )  hasColor    = true;
    kvs::ValueArray<kvs::Real32>         coords      = _ply->coords();  
    kvs::ValueArray<kvs::Real32>         normals     = _ply->normals();
    kvs::ValueArray<kvs::UInt8>          colors      = _ply->colors();

    std::ofstream fout( _filename );
    if ( _type == Ascii ) {
        fout << "#/SPBR_ASCII_Data"       << std::endl;
        fout << "#/RepeatLevel 100"       << std::endl;
        fout << "#/BGColorRGBByte 0 0 0"  << std::endl;
        fout << "#/ImageResolution 1000"  << std::endl;
        fout << "#/LOD 0"                 << std::endl;
        fout << "#/Shading 0"             << std::endl;
        fout << "#/WireframeBox "
                    << _select_area_min_max[0] << " " 
                    << _select_area_min_max[1] << " " 
                    << _select_area_min_max[2] << " " 
                    << _select_area_min_max[3] << " " 
                    << _select_area_min_max[4] << " "
                    << _select_area_min_max[5] << std::endl;
        fout << "#/EndHeader"             << std::endl;
    }

    // Print select area
    std::cout << "\nSelect area Min, Max Coords:" << std::endl;
    std::cout << "Min : "   << _select_area_min_max[0] << " " 
                            << _select_area_min_max[1] << " "
                            << _select_area_min_max[2] << std::endl;
    std::cout << "Max : "   << _select_area_min_max[3] << " " 
                            << _select_area_min_max[4] << " "
                            << _select_area_min_max[5] << std::endl;

    // Write to ouput file
    std::cout << "\nNow writing spbr file..." << std::endl;
    unsigned int count_num_point = 0;
    for ( int i = 0; i < num; i++ ) {
        if ( (_select_area_min_max[0] <= coords[3*i]   && coords[3*i]   <= _select_area_min_max[3]) &&
             (_select_area_min_max[1] <= coords[3*i+1] && coords[3*i+1] <= _select_area_min_max[4]) &&
             (_select_area_min_max[2] <= coords[3*i+2] && coords[3*i+2] <= _select_area_min_max[5]) ) {

            // coords                                               
            float x = coords[3*i];
            float y = coords[3*i+1];
            float z = coords[3*i+2];

            // normal(default)
            float nx = NORMAL[0];
            float ny = NORMAL[1];
            float nz = NORMAL[2];
            if ( hasNormal ) {
                nx = normals[ 3*i ];
                ny = normals[ 3*i+1 ];
                nz = normals[ 3*i+2];
            }

            // color(default)
            int r = COLOR[0];
            int g = COLOR[1];
            int b = COLOR[2];
            if ( hasColor ) {
                r = (int)colors[3*i];
                g = (int)colors[3*i+1];
                b = (int)colors[3*i+2];
            }

            // Binary
            if( _type == Binary ) {
                fout.write( (char*)&x, sizeof(float) );
                fout.write( (char*)&y, sizeof(float) );
                fout.write( (char*)&z, sizeof(float) );
                fout.write( (char*)&nx, sizeof(float) );
                fout.write( (char*)&ny, sizeof(float) );
                fout.write( (char*)&nz, sizeof(float) );

                unsigned char cl = (unsigned char)r;
                fout.write( (char*)&cl, sizeof(unsigned char) );
                cl = (unsigned char)g;
                fout.write( (char*)&cl, sizeof(unsigned char) );
                cl = (unsigned char)b;
                fout.write( (char*)&cl, sizeof(unsigned char) );

                // float tmp = _ni[i]; 
                // fout.write( (char*)&tmp, sizeof(float) );
            
            // Ascii
            } else {     
                fout    << x   << " " << y  << " " << z  << " "
                        << nx  << " " << ny << " " << nz << " "
                        << r   << " " << g  << " " << b  << " " 
                        << std::endl;
            }
            // end if

            count_num_point++;
        } // end if
    } // end for

    std::cout << "\nNumber of points before cropping\n> "   << num << std::endl;
    std::cout << "\nNumber of points after cropping\n> "    << count_num_point << std::endl;
    std::cout << "\nPoint set reduced to "    << int(count_num_point)/int(num)*100 << " (%)" << std::endl;

    fout.close();
}


#endif
