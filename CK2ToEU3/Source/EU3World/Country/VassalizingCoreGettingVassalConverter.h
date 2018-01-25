#ifndef VASSALIZINGCOREGETTINGVASSALCONVERTER_H_
#define VASSALIZINGCOREGETTINGVASSALCONVERTER_H_

#include <memory>
#include "EU3Country.h"
#include "EU3World/EU3Diplomacy.h"

namespace eu3
{
namespace country
{

class VassalizingCoreGettingVassalConverter
{
public:
    VassalizingCoreGettingVassalConverter(EU3Country* liege, EU3Country* vassal) : liege(liege), vassal(vassal)
    {
    };
    void convert(int, EU3Diplomacy*);

private:
    void getVassalCores(int);
    void absorbProvinceIfLiegeCapital(EU3Province*);
    void setAsCore(EU3Province*);
    void addVassalAgreement(EU3Diplomacy*);
    std::shared_ptr<EU3Agreement> createVassalAgreement();
    void setAgreementParties(std::shared_ptr<EU3Agreement>, EU3Diplomacy*);
    EU3Country* liege;
    EU3Country* vassal;
};

} // namespace country
} // namespace eu3
#endif // VASSALIZINGCOREGETTINGVASSALCONVERTER_H_
