// Include the NuSystem header file
#include <nusys.h>

// Define some constants for screen size and color
#define SCREEN_WD 320
#define SCREEN_HT 240
#define CLR_BLACK 0x00000000
#define CLR_WHITE 0xFFFFFFFF

// Declare some global variables for graphics buffers and tasks
Gfx *glistp;
static Gfx glist[2048];
static u16 cfb_16_a[SCREEN_WD*SCREEN_HT];
static u16 cfb_16_b[SCREEN_WD*SCREEN_HT];
NUContData contdata[1];

// Declare some functions for initialization and drawing
void init(void);
void update(void);
void draw(void);

// Main function
void mainproc(void)
{
    // Initialize graphics and controller
    init();

    // Main loop
    while(1)
    {
        // Update controller data
        update();

        // Draw graphics on screen
        draw();

        // Check if start button is pressed to exit game
        if(contdata[0].trigger & START_BUTTON) break;
    }
}

// Initialization function
void init(void)
{
    // Initialize graphics system with default settings
    nuGfxInit();

    // Initialize controller system with one controller connected 
    nuContInit();
}

// Update function 
void update(void)
{
    // Read data from first controller 
    nuContDataGetEx(contdata,0);
}

// Draw function 
void draw(void)
{
    // Get pointer to graphics buffer 
    glistp = glist;

    // Initialize RCP (Reality Co-Processor) state 
    gfxRCPInit();

    // Clear color framebuffer and Z-buffer 
    gfxClearCfb();

    // Set up orthographic projection matrix (2D mode) 
    guOrtho(&gfx_dynamic.projection,
            -(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
            -(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
            1.0F, -1.0F, 1.0F);
    
   gSPPerspNormalize(glistp++, OS_K0_TO_PHYSICAL(&(gfx_dynamic.projection)));
   gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(gfx_dynamic.projection)),
             G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

   gSPViewport(glistp++,&vp);

   gSPDisplayList(glistp++,setup_rdpstate);

   gSPDisplayList(glistp++,setup_rspstate);

   // Draw "Hello World" text on screen using font texture 
   Draw8Font( -56 , -8 , CLR_WHITE , "Hello World");

   // End display list 
   gDPFullSync(glistp++);
   gSPEndDisplayList(glistp++);

   assert((glistp-glist) < GLIST_LEN);

   /* Activate graphic task */
   
#ifdef NU_DEBUG   
      /* Use this only when using NuSystem's graphic debugger */
      nuGfxTaskStart(&glist[0],
                     (s32)(glistp - glist) * sizeof (Gfx),
                     NU_GFX_UCODE_F3DEX ,NU_SC_SWAPBUFFER);
#else /* NU_DEBUG */
      /* Use this when not using NuSystem's graphic debugger */
      nuGfxTaskStart(&glist[0],
                     (s32)(glistp - glist) * sizeof(Gfx),
                     NU_GFX_UCODE_F3DEX ,NU_SC_NOSWAPBUFFER);
#endif /* NU_DEBUG */

      /* Wait until graphic task ends */
      nu
