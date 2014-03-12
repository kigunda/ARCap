--*****************************************************************************
-- ARCap
-- Modified 2014 by Amshu Gongal, Kenan Kigunda
-- Last Updated: February 2, 2014
--*****************************************************************************

--*****************************************************************************
--*  Copyright (c) 2012 by Michael Fischer. All rights reserved.
--*
--*  Redistribution and use in source and binary forms, with or without 
--*  modification, are permitted provided that the following conditions 
--*  are met:
--*  
--*  1. Redistributions of source code must retain the above copyright 
--*     notice, this list of conditions and the following disclaimer.
--*  2. Redistributions in binary form must reproduce the above copyright
--*     notice, this list of conditions and the following disclaimer in the 
--*     documentation and/or other materials provided with the distribution.
--*  3. Neither the name of the author nor the names of its contributors may 
--*     be used to endorse or promote products derived from this software 
--*     without specific prior written permission.
--*
--*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
--*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
--*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
--*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
--*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
--*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
--*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
--*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
--*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
--*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
--*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
--*  SUCH DAMAGE.
--*
--*****************************************************************************
--*  History:
--*
--*  01.08.2012  mifi  First version
--*  15.08.2012  mifi  Added RESET port of the DM9000
--*****************************************************************************

--*****************************************************************************
--*  DEFINE: Library                                                          *
--*****************************************************************************
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


--*****************************************************************************
--*  DEFINE: Entity                                                           *
--*****************************************************************************

entity de0_nano_system is
  port ( 
         -- Input clock 
         CLOCK_50      : in  std_logic;
        
         -- DRAM interface   
         DRAM_CLK     : out   std_logic;                       -- Master Clock
         DRAM_CKE     : out   std_logic;                       -- Clock Enable    
         DRAM_CS_N    : out   std_logic;                       -- Chip Select
         DRAM_RAS_N   : out   std_logic;                       -- Row Address Strobe
         DRAM_CAS_N   : out   std_logic;                       -- Column Address Strobe
         DRAM_WE_N    : out   std_logic;                       -- Write Enable
         DRAM_DQ      : inout std_logic_vector(15 downto 0);   -- Data I/O (16 bits)
			DRAM_DQM		 : out	std_logic_vector(1 downto 0);		-- Output Disable/Write Mask
         DRAM_ADDR    : out   std_logic_vector(12 downto 0);   -- Address Input (13 bits)
			DRAM_BA		 : out	std_logic_vector(1 downto 0);		-- Bank Address

			-- ADC
			ADC_SCLK		: out		std_logic;	-- ADC Clock
			ADC_CS_N		: out		std_logic; 	-- ADC Chip Select
			ADC_SADDR	: out		std_logic;	-- ADC Data Input
			ADC_SDAT		: in		std_logic;	-- ADC Data Output
			
         -- Parallel I/O
         LED     		  : out   std_logic_vector(7 downto 0);	-- LEDs
         KEY           : in    std_logic_vector(1 downto 0);	-- Pushbuttons
         SW            : in    std_logic_vector(3 downto 0);	-- Switches
         GPIO_0		  : inout std_logic_vector(33 downto 0); 	-- 2 x 40 Expansion Header
			GPIO_1		  : inout std_logic_vector(33 downto 0);	-- 2 x 40 Expansion Header
			GPIO_2		  : inout std_logic_vector(1 downto 0)		-- 2 x 13 Expansion Header
       );
end entity de0_nano_system;


--*****************************************************************************
--*  DEFINE: Architecture                                                     *
--*****************************************************************************

architecture syn of de0_nano_system is

   --
   -- Define all components which are included here
   --
   
   component pll_sys
     port ( 
            inclk0   : in  std_logic  := '0';
            c0       : out std_logic ;
            c1       : out std_logic ;
            c2       : out std_logic ;
            locked   : out std_logic 
          );
   end component pll_sys;
   
   
   component heartbeat is
      port (  
             clk         : in  std_logic;
             counter_out : out std_logic
           );
   end component heartbeat;
   

   component system is
      port ( -- Clock and reset
             clk_clk              		: in    std_logic                     	:= 'X';             	-- clk
             reset_reset_n        		: in    std_logic                     	:= 'X';             	-- reset_n				 
             
				 -- DRAM
				 sdram_addr           		: out   std_logic_vector(12 downto 0);                    	-- addr
             sdram_ba             		: out   std_logic_vector(1 downto 0);                     	-- ba
             sdram_cas_n          		: out   std_logic;                                        	-- cas_n
             sdram_cke            		: out   std_logic;                                        	-- cke
             sdram_cs_n           		: out   std_logic;                                        	-- cs_n
             sdram_dq             		: inout std_logic_vector(15 downto 0) 	:= (others => 'X'); 	-- dq
             sdram_dqm            		: out   std_logic_vector(1 downto 0);                     	-- dqm
             sdram_ras_n          		: out   std_logic;                                        	-- ras_n
             sdram_we_n           		: out   std_logic;                                         	-- we_n
             
				 -- ADC
				 adc_ctrl_sclk					: out   std_logic;														-- clk
				 adc_ctrl_cs_n					: out	  std_logic;														-- chip select
				 adc_ctrl_din					: out	  std_logic;														-- data in
				 adc_ctrl_dout					: in	  std_logic;														-- data out
				 
				 -- Parallel I/Os
				 pio_led_export 		 		: out   std_logic_vector(6 downto 0);                     	-- export
             pio_key_left_export  		: in    std_logic								:= 'X';				  	-- export
             pio_sw_export        		: in    std_logic_vector(3 downto 0)  	:= (others => 'X'); 	-- export
				 pio_ir_emitter_export 		: out	  std_logic													  		-- export
           );
   end component system;
   
      
   --
   -- Define all local signals (like static data) here
   --
   signal clk_10     : std_logic;
   signal clk_sys    : std_logic;
   signal pll_locked : std_logic;
	  
begin

   inst_pll_sys : pll_sys
      port map ( 
                 inclk0 => CLOCK_50,
                 c0     => clk_sys,
                 c1     => DRAM_CLK,
                 c2     => clk_10,
                 locked => pll_locked
               );
                                  
   inst_heartbeat : heartbeat 
      port map ( 
                 clk         => clk_10,
                 counter_out => LED(7)
               );   
               
   inst_cpu : system
      port map ( -- Clock and reset
                 clk_clk              		=> clk_sys,
                 reset_reset_n        		=> pll_locked,
                 
					  -- DRAM
                 sdram_addr           		=> DRAM_ADDR,
                 sdram_ba             		=> DRAM_BA,
                 sdram_cas_n          		=> DRAM_CAS_N,
                 sdram_cke            		=> DRAM_CKE,
                 sdram_cs_n           		=> DRAM_CS_N,
                 sdram_dq             		=> DRAM_DQ,
                 sdram_dqm            		=> DRAM_DQM,
                 sdram_ras_n          		=> DRAM_RAS_N,
                 sdram_we_n           		=> DRAM_WE_N,

					  -- ADC
					  adc_ctrl_sclk				=> ADC_SCLK,
					  adc_ctrl_cs_n				=> ADC_CS_N,
					  adc_ctrl_din					=> ADC_SADDR,
					  adc_ctrl_dout				=> ADC_SDAT,
					  
					  -- Parallel I/O
                 pio_led_export 		  		=> LED(6 downto 0),
                 pio_key_left_export  		=> KEY(1),
                 pio_sw_export        		=> SW,
					  pio_ir_emitter_export		=> GPIO_2(1)
               );  
         
end architecture syn;

-- *** EOF ***
