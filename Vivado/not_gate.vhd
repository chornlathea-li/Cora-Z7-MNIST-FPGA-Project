
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity not_gate is
    Port ( i_in : in STD_LOGIC;
           o_out : out STD_LOGIC);
end not_gate;

architecture Behavioral of not_gate is

begin

    o_out <= not i_in;

end Behavioral;