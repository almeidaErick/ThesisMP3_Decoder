----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.09.2017 00:45:12
-- Design Name: 
-- Module Name: COS_TAB1_ROM - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity COS_TAB1_ROM is
--  Port ( );
    Port(
        row : in STD_LOGIC_VECTOR(7 downto 0);
        col : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER
        --table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end COS_TAB1_ROM;

architecture Behavioral of COS_TAB1_ROM is
type ROM is array (0 to 11, 0 to 5) of integer;
constant GET_COS_TAB1_ROM: ROM := 
    (
        (609, -924, -131, 991, -383, -793),
        (383, -924, 924, -383, -383, 924),
        (131, -383, 609, -793, 924, -991),
        (-131, 383, -609, 793, -924, 991),
        (-383, 924, -924, 383, 383, -924),
        (-609, 924, 131, -991, 383, 793),
        (-793, 383, 991, 131, -924, -609),
        (-924, -383, 383, 924, 924, 383),
        (-991, -924, -793, -609, -383, -131),
        (-991, -924, -793, -609, -383, -131),
        (-924, -383, 383, 924, 924, 383),
        (-793, 383, 991, 131, -924, -609)
    );

begin
    process(clock)
    begin
        if rising_edge(clock) then
            --if (table_select = b"01") then
                output_data <= GET_COS_TAB1_ROM((to_integer(unsigned(row))),(to_integer(unsigned(col)))); -- synchronous reads
            --end if; 
        end if;
    end process;
--output_data <= GET_COS_TAB1_ROM(to_integer(unsigned(row)))(to_integer(unsigned(col)));  -- read from COS_TAB1 table (asynchronous reads)
end Behavioral;
