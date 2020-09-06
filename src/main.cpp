#include "DRIRSofaReader.hpp"

int main()
{
    DRIRSofaReader sfReader("data/Marshall1960ADRIR.sofa");
    auto ir = sfReader.getDRIRs().row(0);
    for (std::size_t i = 0; i < ir.size(); i++)
        std::cout << i << " " << ir(0) << std::endl;
}
