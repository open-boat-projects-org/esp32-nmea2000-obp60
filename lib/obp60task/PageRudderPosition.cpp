#include "Pagedata.h"
#include "OBP60ExtensionPort.h"

class PageRudderPosition : public Page
{
bool keylock = false;               // Keylock

public:
    PageRudderPosition(CommonData &common){
        common.logger->logDebug(GwLog::LOG,"Show PageRudderPosition");
    }

    // Key functions
    virtual int handleKey(int key){
        // Keylock function
        if(key == 11){              // Code for keylock
            keylock = !keylock;     // Toggle keylock
            return 0;               // Commit the key
        }
        return key;
    }

    virtual void displayPage(CommonData &commonData, PageData &pageData)
    {
        GwConfigHandler *config = commonData.config;
        GwLog *logger=commonData.logger;

        static String unit1old = "";
        double value1 = 0.1;
        double value1old = 0.1;

        // Get config data
        String lengthformat = config->getString(config->lengthFormat);
        bool simulation = config->getBool(config->useSimuData);
        String displaycolor = config->getString(config->displaycolor);
        bool holdvalues = config->getBool(config->holdvalues);
        String flashLED = config->getString(config->flashLED);
        String backlightMode = config->getString(config->backlight);
        int timezone = config->getInt(config->timeZone);

        // Get boat values for rudder position
        GwApi::BoatValue *bvalue1 = pageData.values[0]; // First element in list
        String name1 = bvalue1->getName().c_str();      // Value name
        name1 = name1.substring(0, 6);                  // String length limit for value name
        value1 = bvalue1->value;                        // Raw value without unit convertion
        bool valid1 = bvalue1->valid;                   // Valid information 
        String svalue1 = formatValue(bvalue1, commonData).svalue;    // Formatted value as string including unit conversion and switching decimal places
        String unit1 = formatValue(bvalue1, commonData).unit;        // Unit of value
        if(valid1 == true){
            value1old = value1;   	                    // Save old value
            unit1old = unit1;                           // Save old unit
        }

        // Optical warning by limit violation (unused)
        if(String(flashLED) == "Limit Violation"){
            setBlinkingLED(false);
            setPortPin(OBP_FLASH_LED, false); 
        }

        // Logging boat values
        if (bvalue1 == NULL) return;
        LOG_DEBUG(GwLog::LOG,"Drawing at PageRudderPosition, %s:%f", name1, value1);

        // Draw page
        //***********************************************************

        // Set background color and text color
        int textcolor = GxEPD_BLACK;
        int pixelcolor = GxEPD_BLACK;
        int bgcolor = GxEPD_WHITE;
        if(displaycolor == "Normal"){
            textcolor = GxEPD_BLACK;
            pixelcolor = GxEPD_BLACK;
            bgcolor = GxEPD_WHITE;
        }
        else{
            textcolor = GxEPD_WHITE;
            pixelcolor = GxEPD_WHITE;
            bgcolor = GxEPD_BLACK;
        }
        // Clear display by call in obp60task.cpp in main loop


//*******************************************************************************************
        
        // Draw RudderPosition
        int rWindGraphic = 110;     // Radius of RudderPosition
        float pi = 3.141592;

        display.fillCircle(200, 150, rWindGraphic + 10, pixelcolor);    // Outer circle
        display.fillCircle(200, 150, rWindGraphic + 7, bgcolor);        // Outer circle
        display.fillRect(0, 30, 400, 122, bgcolor);                      // Delete half top circle

        for(int i=90; i<=270; i=i+10)
        {
            // Scaling values
            float x = 200 + (rWindGraphic-30)*sin(i/180.0*pi);  //  x-coordinate dots
            float y = 150 - (rWindGraphic-30)*cos(i/180.0*pi);  //  y-coordinate cots 
            const char *ii = " ";
            switch (i)
            {
            case 0: ii=" "; break;      // Use a blank for a empty scale value
            case 30 : ii=" "; break;
            case 60 : ii=" "; break;
            case 90 : ii="45"; break;
            case 120 : ii="30"; break;
            case 150 : ii="15"; break;
            case 180 : ii="0"; break;
            case 210 : ii="15"; break;
            case 240 : ii="30"; break;
            case 270 : ii="45"; break;
            case 300 : ii=" "; break;
            case 330 : ii=" "; break;
            default: break;
            }

            // Print text centered on position x, y
            int16_t x1, y1;     // Return values of getTextBounds
            uint16_t w, h;      // Return values of getTextBounds
            display.getTextBounds(ii, int(x), int(y), &x1, &y1, &w, &h); // Calc width of new string
            display.setCursor(x-w/2, y+h/2);
            if(i % 30 == 0){
                display.setFont(&Ubuntu_Bold8pt7b);
                display.print(ii);
            }

            // Draw sub scale with dots
            float x1c = 200 + rWindGraphic*sin(i/180.0*pi);
            float y1c = 150 - rWindGraphic*cos(i/180.0*pi);
            display.fillCircle((int)x1c, (int)y1c, 2, pixelcolor);
            float sinx=sin(i/180.0*pi);
            float cosx=cos(i/180.0*pi); 

            // Draw sub scale with lines (two triangles)
            if(i % 30 == 0){
                float dx=2;   // Line thickness = 2*dx+1
                float xx1 = -dx;
                float xx2 = +dx;
                float yy1 =  -(rWindGraphic-10);
                float yy2 =  -(rWindGraphic+10);
                display.fillTriangle(200+(int)(cosx*xx1-sinx*yy1),150+(int)(sinx*xx1+cosx*yy1),
                        200+(int)(cosx*xx2-sinx*yy1),150+(int)(sinx*xx2+cosx*yy1),
                        200+(int)(cosx*xx1-sinx*yy2),150+(int)(sinx*xx1+cosx*yy2),pixelcolor);
                display.fillTriangle(200+(int)(cosx*xx2-sinx*yy1),150+(int)(sinx*xx2+cosx*yy1),
                        200+(int)(cosx*xx1-sinx*yy2),150+(int)(sinx*xx1+cosx*yy2),
                        200+(int)(cosx*xx2-sinx*yy2),150+(int)(sinx*xx2+cosx*yy2),pixelcolor);  
            }

        }

        // Print label
        display.setTextColor(textcolor);
        display.setFont(&Ubuntu_Bold16pt7b);
        display.setCursor(80, 70);
        display.print("Rudder Position");               // Label

        // Print Unit in RudderPosition
        if(holdvalues == false){
            display.setFont(&Ubuntu_Bold12pt7b);
            display.setCursor(175, 110);
            display.print(unit1);                       // Unit
        }
        else{
            display.setFont(&Ubuntu_Bold12pt7b);
            display.setCursor(175, 110);
            display.print(unit1old);                    // Unit
        }

        // Calculate rudder position
        if(holdvalues == true && valid1 == false){
            value1 = 2 * pi - ((value1old * 2) + pi);
        }
        else{
            value1 = 2 * pi - ((value1 * 2) + pi);
        }

        // Draw rudder position pointer
        float startwidth = 8;       // Start width of pointer

        if(valid1 == true || holdvalues == true || simulation == true){
            float sinx=sin(value1);
            float cosx=cos(value1);
            // Normal pointer
            // Pointer as triangle with center base 2*width
            float xx1 = -startwidth;
            float xx2 = startwidth;
            float yy1 = -startwidth;
            float yy2 = -(rWindGraphic * 0.5); 
            display.fillTriangle(200+(int)(cosx*xx1-sinx*yy1),150+(int)(sinx*xx1+cosx*yy1),
                200+(int)(cosx*xx2-sinx*yy1),150+(int)(sinx*xx2+cosx*yy1),
                200+(int)(cosx*0-sinx*yy2),150+(int)(sinx*0+cosx*yy2),pixelcolor);   
            // Inverted pointer
            // Pointer as triangle with center base 2*width
            float endwidth = 2;         // End width of pointer
            float ix1 = endwidth;
            float ix2 = -endwidth;
            float iy1 = -(rWindGraphic * 0.5);
            float iy2 = -endwidth;
            display.fillTriangle(200+(int)(cosx*ix1-sinx*iy1),150+(int)(sinx*ix1+cosx*iy1),
                200+(int)(cosx*ix2-sinx*iy1),150+(int)(sinx*ix2+cosx*iy1),
                200+(int)(cosx*0-sinx*iy2),150+(int)(sinx*0+cosx*iy2),pixelcolor);
        }

        // Center circle
        display.fillCircle(200, 150, startwidth + 6, bgcolor);
        display.fillCircle(200, 150, startwidth + 4, pixelcolor);

//*******************************************************************************************
        // Key Layout
        display.setTextColor(textcolor);
        display.setFont(&Ubuntu_Bold8pt7b);
        display.setCursor(130, 290);
        if(keylock == false){
            display.print("[  <<<<  " + String(commonData.data.actpage) + "/" + String(commonData.data.maxpage) + "  >>>>  ]");
            if(String(backlightMode) == "Control by Key"){              // Key for illumination
                display.setCursor(343, 290);
                display.print("[ILUM]");
            }
        }
        else{
            display.print(" [    Keylock active    ]");
        }

        // Update display
        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, true);    // Partial update (fast)

    };
};

static Page *createPage(CommonData &common){
    return new PageRudderPosition(common);
}
/**
 * with the code below we make this page known to the PageTask
 * we give it a type (name) that can be selected in the config
 * we define which function is to be called
 * and we provide the number of user parameters we expect (0 here)
 * and will will provide the names of the fixed values we need
 */
PageDescription registerPageRudderPosition(
    "RudderPosition",   // Page name
    createPage,         // Action
    0,                  // Number of bus values depends on selection in Web configuration
    {"RPOS"},           // Bus values we need in the page
    true                // Show display header on/off
);