
const uint8_t sinetbl_64[256] PROGMEM = {
    15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19,
    20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24,
    24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 24, 24,
    24, 24, 23, 23, 23, 23, 22, 22, 22, 22, 21, 21, 21, 21, 20, 20,
    20, 19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15,
    15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 11, 10,
    10, 10, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6,
    6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6,
    6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15,
};
const uint8_t sinetbl_128[256] PROGMEM = {
    30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39,
    40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48,
    48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54,
    54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56,
    56, 56, 56, 56, 56, 56, 56, 55, 55, 55, 55, 55, 55, 55, 54, 54,
    54, 54, 53, 53, 53, 52, 52, 52, 52, 51, 51, 50, 50, 50, 49, 49,
    48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 43, 43, 42, 42, 41, 41,
    40, 39, 39, 38, 38, 37, 36, 36, 35, 35, 34, 33, 33, 32, 31, 31,
    30, 30, 29, 28, 28, 27, 26, 26, 25, 25, 24, 23, 23, 22, 22, 21,
    20, 20, 19, 19, 18, 18, 17, 17, 16, 15, 15, 14, 14, 13, 13, 13,
    12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7,
    7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12,
    12, 13, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20,
    20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30,
};
const uint8_t sinetbl_176[256] PROGMEM = {
    42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 54,
    55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66,
    67, 67, 68, 68, 69, 69, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74,
    74, 75, 75, 75, 75, 76, 76, 76, 76, 76, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 76, 76, 76, 76, 76, 75, 75, 75, 75,
    74, 74, 74, 73, 73, 72, 72, 71, 71, 71, 70, 69, 69, 68, 68, 67,
    67, 66, 65, 65, 64, 63, 63, 62, 61, 61, 60, 59, 58, 58, 57, 56,
    55, 54, 54, 53, 52, 51, 50, 49, 49, 48, 47, 46, 45, 44, 43, 43,
    42, 41, 40, 39, 38, 37, 37, 36, 35, 34, 33, 32, 32, 31, 30, 29,
    28, 27, 27, 26, 25, 24, 24, 23, 22, 21, 21, 20, 19, 19, 18, 17,
    17, 16, 16, 15, 15, 14, 13, 13, 12, 12, 12, 11, 11, 10, 10, 10,
    9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9,
    9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 15, 15, 16, 16,
    17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 27,
    28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41,
};
const uint8_t sinetbl_240[256] PROGMEM = {
    57, 58, 59, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 74,
    75, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90,
    91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 98, 99, 99, 100, 100, 101,
    101, 102, 102, 103, 103, 103, 104, 104, 104, 104, 105, 105, 105, 105, 105, 105,
    105, 105, 105, 105, 105, 105, 105, 104, 104, 104, 104, 103, 103, 103, 102, 102,
    101, 101, 100, 100, 99, 99, 98, 98, 97, 96, 96, 95, 94, 93, 93, 92,
    91, 90, 89, 88, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,
    75, 74, 73, 72, 71, 70, 69, 68, 66, 65, 64, 63, 62, 61, 59, 58,
    57, 56, 55, 54, 52, 51, 50, 49, 48, 47, 45, 44, 43, 42, 41, 40,
    39, 38, 37, 36, 34, 33, 32, 31, 30, 29, 29, 28, 27, 26, 25, 24,
    23, 22, 22, 21, 20, 19, 19, 18, 17, 17, 16, 15, 15, 14, 14, 13,
    13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12,
    13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 22, 22,
    23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 47, 48, 49, 50, 51, 52, 54, 55, 56,
};
const uint8_t sinetbl_320[256] PROGMEM = {
    76, 78, 79, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 98, 99,
    101, 102, 104, 105, 106, 108, 109, 111, 112, 113, 114, 116, 117, 118, 119, 120,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 130, 131, 132, 133, 133, 134, 135,
    135, 136, 137, 137, 138, 138, 138, 139, 139, 139, 140, 140, 140, 140, 140, 140,
    140, 140, 140, 140, 140, 140, 140, 139, 139, 139, 138, 138, 138, 137, 137, 136,
    135, 135, 134, 133, 133, 132, 131, 130, 130, 129, 128, 127, 126, 125, 124, 123,
    122, 120, 119, 118, 117, 116, 114, 113, 112, 111, 109, 108, 106, 105, 104, 102,
    101, 99, 98, 96, 95, 93, 92, 90, 89, 87, 86, 84, 83, 81, 79, 78,
    76, 75, 73, 72, 70, 68, 67, 65, 64, 62, 61, 59, 58, 56, 55, 53,
    52, 50, 49, 48, 46, 45, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32,
    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 21, 20, 19, 18, 18,
    17, 17, 16, 16, 15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12,
    12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17,
    17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 45, 46, 48, 49, 50,
    52, 53, 55, 56, 58, 59, 61, 62, 64, 65, 67, 68, 70, 72, 73, 75,
};
const uint8_t sinetbl_480[256] PROGMEM = {
    115, 117, 119, 122, 124, 126, 129, 131, 133, 136, 138, 140, 143, 145, 147, 149,
    151, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 177, 179, 181,
    183, 184, 186, 187, 189, 190, 192, 193, 195, 196, 197, 198, 199, 200, 201, 202,
    203, 204, 205, 206, 207, 207, 208, 208, 209, 209, 210, 210, 210, 210, 211, 211,
    211, 211, 211, 210, 210, 210, 210, 209, 209, 208, 208, 207, 207, 206, 205, 204,
    203, 202, 201, 200, 199, 198, 197, 196, 195, 193, 192, 190, 189, 187, 186, 184,
    183, 181, 179, 177, 176, 174, 172, 170, 168, 166, 164, 162, 160, 158, 156, 154,
    151, 149, 147, 145, 143, 140, 138, 136, 133, 131, 129, 126, 124, 122, 119, 117,
    115, 112, 110, 108, 105, 103, 101, 98, 96, 94, 91, 89, 87, 85, 82, 80,
    78, 76, 74, 72, 69, 67, 65, 63, 61, 59, 58, 56, 54, 52, 50, 49,
    47, 45, 44, 42, 40, 39, 38, 36, 35, 34, 32, 31, 30, 29, 28, 27,
    26, 25, 24, 24, 23, 22, 22, 21, 21, 20, 20, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 34, 35, 36, 38, 39, 40, 42, 44, 45,
    47, 49, 50, 52, 54, 56, 58, 59, 61, 63, 65, 67, 69, 72, 74, 76,
    78, 80, 82, 85, 87, 89, 91, 94, 96, 98, 101, 103, 105, 108, 110, 112,
};