-- Kenan Kigunda, Amshu Gongal, Matt Pon
-- February 7, 2014

-- DERIVED FROM monolith_sev_seg.vhd
-- Nancy Minderman 
-- Created December 17, 2013
-- Makes much use of the Altera "Insert Template" feature
-- nancy.minderman@ualberta.ca
-- Modified January 13, 2014 to use the DE2_CONSTANTS package
-- Makes much use of the Altera "Insert Template" feature
-- nancy.minderman@ualberta.ca
-- Modified January 13, 2014 to use the DE2_CONSTANTS package

-- A library clause declares a name as a library.  It 
-- does not create the library; it simply forward declares 
-- it. 
library ieee;

-- Use clauses import declarations into the current scope.	
-- If more than one use clause imports the same name into the
-- the same scope, none of the names are imported.


-- Commonly imported packages:

-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
use ieee.std_logic_1164.all;

-- SIGNED and UNSIGNED types, and relevant functions
use ieee.numeric_std.all;

-- Basic sequential functions and concurrent procedures
use ieee.VITAL_Primitives.all;

use work.DE0_CONSTANTS.all;

entity infrared_controller is

	port(
		-- clock and reset
		clk		: in std_logic;
		reset_n 	: in std_logic;
		
		-- signal to infrared emitter and receivers
		coe_emitter	: out std_logic;
		coe_receiver : in std_logic_vector(3 downto 0);
		
		-- avalon slave interface, for controlling the emitters and receivers
		avs_s0_readdata : out std_logic_vector(31 downto 0);
		avs_s0_writedata : in std_logic_vector(31 downto 0);
		avs_s0_write_n : in std_logic
	);

end infrared_controller;

architecture avalon of infrared_controller is
begin
	-- Watch the reset and write signals.
	process(reset_n, avs_s0_write_n) is
	begin
		if (reset_n = '0') then
			-- Reset.
		elsif (avs_s0_write_n = '0') then
			coe_emitter <= avs_s0_writedata(0);
		end if;
	end process;
	-- Output the count.
	avs_s0_readdata(3 downto 0) <= coe_receiver;
end avalon;

