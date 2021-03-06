xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}


Material Default {
 1.000000;1.000000;1.000000;1.000000;;
 50.000000;
 0.020000;0.020000;0.020000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "eye_fxl_t_lightning_014.dds";
 }
}

Frame Plane01 {
 

 FrameTransformMatrix {
  0.393701,0.000000,0.000000,0.000000,0.000000,-0.000000,-0.393701,0.000000,0.000000,0.393701,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh Plane01 {
  27;
  5.335766;-15.395123;3.389617;,
  5.335766;0.000000;6.931840;,
  -7.355724;0.000000;-12.368700;,
  -7.355724;-16.328176;6.839055;,
  -7.355724;16.328176;14.749992;,
  5.335766;15.395123;-5.630104;,
  18.027256;0.000000;19.385091;,
  18.027256;14.462069;0.885366;,
  30.718746;0.000000;23.864912;,
  30.718746;13.529014;15.591862;,
  43.410237;0.000000;25.037581;,
  43.410237;12.595961;21.014277;,
  56.101728;0.000000;9.576730;,
  56.101728;11.662906;-7.804358;,
  68.793216;0.000000;3.550901;,
  68.793216;10.729852;-11.712092;,
  81.484710;0.000000;-5.321075;,
  81.484710;9.796798;5.938234;,
  18.027256;-14.462069;4.338227;,
  30.718746;-13.529014;7.812071;,
  43.410237;-12.595961;14.509837;,
  56.101728;-11.662906;0.387804;,
  68.793216;-10.729852;0.602872;,
  81.484710;-9.796798;5.408040;,
  94.176192;-8.863743;-21.720126;,
  94.176192;0.000000;-17.298154;,
  94.176192;8.863743;-6.040747;;
  32;
  3;0,1,2;,
  3;2,3,0;,
  3;4,2,1;,
  3;5,4,1;,
  3;1,6,5;,
  3;7,5,6;,
  3;6,8,7;,
  3;9,7,8;,
  3;8,10,9;,
  3;11,9,10;,
  3;10,12,11;,
  3;13,11,12;,
  3;12,14,13;,
  3;15,13,14;,
  3;14,16,15;,
  3;17,15,16;,
  3;0,18,1;,
  3;6,1,18;,
  3;18,19,6;,
  3;8,6,19;,
  3;19,20,8;,
  3;10,8,20;,
  3;20,21,10;,
  3;12,10,21;,
  3;21,22,12;,
  3;14,12,22;,
  3;22,23,14;,
  3;16,14,23;,
  3;23,24,16;,
  3;24,25,16;,
  3;17,16,25;,
  3;25,26,17;;

  MeshNormals {
   27;
   -0.328945;0.174848;0.928021;,
   -0.456639;-0.177131;0.871840;,
   -0.639660;-0.020719;0.768379;,
   0.119158;0.756479;0.643071;,
   0.183941;-0.304506;0.934581;,
   -0.027407;0.699538;0.714070;,
   -0.472860;-0.004781;0.881125;,
   -0.400168;0.664206;0.631424;,
   -0.352007;-0.103127;0.930299;,
   -0.392704;0.412174;0.822129;,
   0.350541;-0.122066;0.928558;,
   0.496735;0.407701;0.766182;,
   0.620488;0.040907;0.783149;,
   0.451201;0.736897;0.503390;,
   0.277572;0.511454;0.813246;,
   -0.049188;0.542815;0.838411;,
   0.538140;0.086252;0.838430;,
   0.120035;-0.760369;0.638303;,
   -0.268100;-0.545669;0.793957;,
   -0.210535;-0.733418;0.646354;,
   0.120003;-0.682643;0.720831;,
   0.491978;-0.544729;0.679138;,
   0.043407;-0.395667;0.917368;,
   0.398630;0.371755;0.838387;,
   0.802762;0.050032;0.594197;,
   0.555231;-0.568887;0.606701;,
   0.465603;-0.695327;0.547480;;
   32;
   3;0,1,2;,
   3;2,3,0;,
   3;4,2,1;,
   3;5,4,1;,
   3;1,6,5;,
   3;7,5,6;,
   3;6,8,7;,
   3;9,7,8;,
   3;8,10,9;,
   3;11,9,10;,
   3;10,12,11;,
   3;13,11,12;,
   3;12,14,13;,
   3;15,13,14;,
   3;14,16,15;,
   3;17,15,16;,
   3;0,18,1;,
   3;6,1,18;,
   3;18,19,6;,
   3;8,6,19;,
   3;19,20,8;,
   3;10,8,20;,
   3;20,21,10;,
   3;12,10,21;,
   3;21,22,12;,
   3;14,12,22;,
   3;22,23,14;,
   3;16,14,23;,
   3;23,24,16;,
   3;24,25,16;,
   3;17,16,25;,
   3;25,26,17;;
  }

  MeshMaterialList {
   1;
   32;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { Default }
  }

  MeshTextureCoords c1 {
   27;
   0.712683;1.125000;,
   0.551908;1.125000;,
   0.523850;1.000000;,
   0.634119;1.000000;,
   0.413580;1.000000;,
   0.391134;1.125000;,
   0.585578;1.250000;,
   0.371493;1.250000;,
   0.585578;1.375000;,
   0.320988;1.375000;,
   0.585578;1.500000;,
   0.239619;1.500000;,
   0.585578;1.625000;,
   0.239618;1.625000;,
   0.585578;1.750000;,
   0.239618;1.750000;,
   0.585578;1.875000;,
   0.239618;1.875000;,
   0.799664;1.250000;,
   0.850169;1.375000;,
   0.931538;1.500000;,
   0.931538;1.625000;,
   0.931538;1.750000;,
   0.931538;1.875000;,
   0.931538;2.000000;,
   0.585578;2.000000;,
   0.239618;2.000000;;
  }
 }
}