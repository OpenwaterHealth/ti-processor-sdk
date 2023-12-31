#ifndef _VALETAPP_PROJECTION_MATRIX_
#define _VALETAPP_PROJECTION_MATRIX_

/* projection matrix to map parking spot on ground plane */


// Sequence 16
double ValetApp_projection_matrix[12] = {
        -0.0370342771198098,     0.851584581647000,    1.040238195886740,    0.0,
        -0.3706994351336200,     1.386872073055660,    0.989116628168284,    0.0,
        -0.00000624740230713446, 0.000254219649153303, 0.000182713676343543, 0.0};

/*
// Sequence 107
double ValetApp_projection_matrix[12] = {
        -0.0347065402113454,	 0.883364568015967,    1.07909480570322,     0.0,
        -0.368427878613101, 	 1.36980827814977,     0.983029544628474,    0.0,
        -0.00000567996584281988,	 0.000250723446748885, 0.000183012669010032, 0.0};
*/

#endif