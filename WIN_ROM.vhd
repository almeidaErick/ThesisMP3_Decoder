----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 26.09.2017 23:49:58
-- Design Name: 
-- Module Name: WIN_ROM - Behavioral
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

entity WIN_ROM is
--  Port ( );
    Port(
        row : in STD_LOGIC_VECTOR(7 downto 0);
        col : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER;
        table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end WIN_ROM;

architecture Behavioral of WIN_ROM is
--type ROM_COL is array (0 to 36) of integer;
type ROM is array (0 to 3, 0 to 35) of integer;
constant GET_WIN_ROM: ROM := 
    (
        (44, 131, 216, 301, 383, 462,
        537, 609, 676, 737, 793, 843, 
        887, 924, 954, 976, 991, 999,
        999, 991, 976, 954, 924, 887,
        843, 793, 737, 676, 609, 537, 
        462, 383, 301, 216, 131, 44),
        
        --type1:win[1][i]
        (44, 131, 216, 301, 383, 462,
        537, 609, 676, 737, 793, 843, 
        887, 924, 954, 976, 991, 999,
        1000, 1000, 1000, 1000, 1000, 1000,
        991, 924, 793, 609, 383, 131, 
        0, 0, 0, 0, 0, 0),
         
        --type2:win[2][i]
        (131, 383, 609, 793, 924, 991,
        991, 924, 793, 609, 383, 131, 
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0),
        
        --type3: win[3][i]
        (0, 0, 0, 0, 0, 0,
        131, 383, 609, 793, 924, 991, 
        1000, 1000, 1000, 1000, 1000, 1000,
        999, 991, 976, 954, 924, 887,
        843, 793, 737, 676, 609, 537, 
        462, 383, 301, 216, 131, 44)
    );
begin
    process(clock)
    begin
        if rising_edge(clock) then
            if (table_select = b"10") then
                output_data <= GET_WIN_ROM((to_integer(unsigned(row))),(to_integer(unsigned(col)))); -- synchronous reads
            end if; 
        end if;
    end process;
--output_data <= GET_WIN_ROM(to_integer(unsigned(row)))(to_integer(unsigned(col)));  -- read from WIN table (asynchronous)

end Behavioral;
