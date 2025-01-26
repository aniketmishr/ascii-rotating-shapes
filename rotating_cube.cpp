#include <iostream>
#include <math.h>
#include <string>
#include <windows.h>
#include<vector>
#include<sstream>
using namespace std; 


const int screen_width = 160;
const int screen_height = 44; 
const float K2 = 100; 
const float K1 =50;
const float cubewidth = 20;  
const float resolution = 0.2; 
const float pi = 3.14159265359; 
const float light[3] = {0,1,-1}; // |light| = sqrt(2)
float A = 1.0, B= 0.5, C= 0.0; 
string CHARS_LUMINANCE =  ".~!*=#";

vector<string> output(screen_width*screen_height, " ");
vector<float> zbuffer(screen_width*screen_height, 0);

void plotPlane(float x, float y, float z, int nx, int ny, int nz, string c)
{
    float cosA = cos(A), sinA = sin(A); 
    float cosB = cos(B), sinB = sin(B); 
    float cosC = cos(C), sinC = sin(C); 

    // rotate the above point 
    float rotatedX  = y * sinA * sinB * cosC - z * cosA * sinB * cosC + y * cosA * sinC + z * sinA * sinC + x * cosB * cosC; 
    float rotatedY = y * cosA * cosC + z * sinA * cosC - y * sinA * sinB * sinC + z * cosA * sinB * sinC - x * cosB * sinC; 
    float rotatedZ  = z * cosA * cosB - y * sinA * cosB + x * sinB;; 
    float ooz = 1/(rotatedZ+K2);

    // find projection over screen 
    int xp = (int) (screen_width/2 + K1*ooz*rotatedX*2);
    int yp = (int) (screen_height/2 - K1*ooz*rotatedY); 
    int position = xp + yp * screen_width;

    float rotatedNx  = ny * sinA * sinB * cosC - nz * cosA * sinB * cosC + ny * cosA * sinC + nz * sinA * sinC + nx * cosB * cosC;; 
    float rotatedNy = ny * cosA * cosC + nz * sinA * cosC - ny * sinA * sinB * sinC + nz * cosA * sinB * sinC - nx * cosB * sinC; 
    float rotatedNz  = nz * cosA * cosB - ny * sinA * cosB + nx * sinB;;
    
    //find Luminance
    float L = rotatedNx*light[0]+rotatedNy*light[1]+rotatedNz*light[2];

    //update frame_buffer
    if(ooz > zbuffer[position]) {
    zbuffer[position] = ooz;
    int luminance_index = L*4;
    output[position] = c + CHARS_LUMINANCE[luminance_index>0 ? luminance_index: 0];// + "\033[0m";

    }
}


int main()
{
    bool rotate = true; 
    ios::sync_with_stdio(false);
    while(1)
    {
        fill(output.begin(), output.end(), " ");
        fill(zbuffer.begin(), zbuffer.end(), 0);

        for (float a = -cubewidth; a<=cubewidth; a+=resolution)
        {
            for (float b = -cubewidth; b<= cubewidth; b+=resolution)
            {
                plotPlane(a,b, cubewidth, 0,0,1, "\033[31m");
                plotPlane(a,b, -cubewidth, 0, 0,-1, "\033[33m");
                plotPlane(cubewidth, a,b, 1,0,0, "\033[93m"); 
                plotPlane(-cubewidth,a,b,-1,0,0, "\033[97m");
                plotPlane(a, cubewidth, b, 0,1,0, "\033[34m");
                plotPlane(a, -cubewidth, b, 0,-1,0, "\033[32m");
            }
        }

        cout<<"\x1b[H";
        ostringstream buffer;
        
        for (int k = 0; k < screen_width*screen_height; ++k) {
            if (k % screen_width == 0) {
                buffer << '\n';
            } else {
                buffer << output[k];
            }
        }
        cout << buffer.str();
 
        // Check if a key is pressed
        if(rotate)
        {
            A+= 0.02; 
            B+= 0.1;   
            C+=0.05; 
        }else{
            if (GetAsyncKeyState('W') & 0x8000) {
            A-=0.1;
            }
            if (GetAsyncKeyState('A') & 0x8000) {
            B-=0.1;
            }
            if (GetAsyncKeyState('S') & 0x8000) {
            A+=0.1; 
            }
            if (GetAsyncKeyState('D') & 0x8000) {
            B+=0.1;
            }
            if (GetAsyncKeyState('Q') & 0x8000) {
            C-=0.1; 
            }
            if (GetAsyncKeyState('E') & 0x8000) {
            C+=0.1;
            }
        }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            rotate = !(rotate);
        }
        // Exit on 'Esc' key press
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            cout << "Exiting...\n";
            break;
        }
        Sleep(20);
    }

}