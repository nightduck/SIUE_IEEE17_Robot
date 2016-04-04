

#define         TIMES                           4096                    // 12-bit PWM

#define         PRU_R31_VEC_VALID               32                      // allows notification of program completion
#define         PRU_EVTOUT_1                    4                       // event number that is sent back for PRU 1 to ARM interrupt
#define         PRU0_PRU1_INTERRUPT             17                      // PRU0->PRU1 interrupt number
#define         PRU1_PRU0_INTERRUPT             18                      // PRU1->PRU0 interrupt number
#define         ARM_PRU1_INTERRUPT              37

// Define linux space GPIOs
#define         GPIO0                           0x44e07000
#define         GPIO1                           0x4804C000
#define         GPIO2                           0x481AC000
#define         GPIO3                           0x481AE000
#define         GPIO_CLEARDATAOUT               0x190                   //Clearing GPIO
#define         GPIO_SETDATAOUT                 0x194                   //Setting GPIO
#define         GPIO_DATAOUT                    0x138                   //reading GPIO
#define         GPIO1_15_MASK                   0x80                    //SWITCH
#define         GPIO2_15_MASK                   0x80                    //LED
#define         GPIO_BUTTON                     r15
#define         GPIO_LED                        r16
#define         read_gpio1                      r17
#define         set_gpio2                       r18
#define         clr_gpio2                       r19


.macro			GPIO_SETUP

			lbco	r0, c4, 4, 4     // load SYSCFG reg into r0 (use c4 const addr)
                        clr     r0, r0, 4        // clear bit 4 (STANDBY_INIT)
                        sbco    r0, c4, 4, 4     // store the modified r0 back at the load addr  
                        mov     read_gpio1, GPIO1 | GPIO_DATAOUT
                        mov     set_gpio2, GPIO2 | GPIO_SETDATAOUT
                        mov     clr_gpio2, GPIO2 | GPIO_CLEARDATAOUT
.endm

.macro			LED_ON
	                set    GPIO_LED.b1, GPIO_LED.b1, GPIO2_15_MASK
                        sbbo   GPIO_LED, set_gpio2, 0, 4
.endm

.macro			BUTTON_CHK
	                lbbo   GPIO_BUTTON, read_gpio1, 0, 4
                        //qbbc   CHECK, GPIO_BUTTON.t15
.endm

.macro			LED_OFF
                        set     GPIO_LED.b1, GPIO_LED.b1, GPIO2_15_MASK
                        sbbo    GPIO_LED, clr_gpio2, 0, 4
.endm

.macro			LED_TOGGLE
			sbbo	GPIO_LED, clr_gpio2, 0, 4
			xor 	GPIO_LED.b1, GPIO_LED.b1, GPIO2_15_MASK
			sbbo	GPIO_LED, set_gpio2, 0, 4
.endm
