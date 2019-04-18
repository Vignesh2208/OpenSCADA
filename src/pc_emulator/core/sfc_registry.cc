#include <assert.h>
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/sfc_registry.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

SFCRegistry::SFCRegistry(PCResourceImpl * AssociatedResource) {
    std::vector<PCDataType*> ElementaryDatatypes;

    AssociatedResource->__configuration
                    ->RegisteredDataTypes
                    ->GetAllRegisteredElementaryDataTypes(
                        ElementaryDatatypes);

    for (int i = 0; i < ElementaryDatatypes.size(); i++) {

        for (int j = 0; j < ElementaryDatatypes.size(); j ++) {
            if(ElementaryDatatypes[i]->__DataTypeCategory 
                <= DataTypeCategory::LREAL
                && ElementaryDatatypes[j]->__DataTypeCategory 
                <= DataTypeCategory::LREAL) {

                string conv_block_name 
                = ElementaryDatatypes[i]->__DataTypeName + "_TO_"
                  + ElementaryDatatypes[j]->__DataTypeName;
                __SFC.insert(std::make_pair(conv_block_name,
                    std::make_unique<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

            if (ElementaryDatatypes[i]->__DataTypeCategory 
                == DataTypeCategory::DATE_AND_TIME
                && ElementaryDatatypes[j]->__DataTypeCategory 
                == DataTypeCategory::DATE) {
                __SFC.insert(std::make_pair("DATE_AND_TIME_TO_DATE",
                    std::make_unique<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

            if (ElementaryDatatypes[i]->__DataTypeCategory 
                == DataTypeCategory::DATE_AND_TIME
                && ElementaryDatatypes[j]->__DataTypeCategory 
                == DataTypeCategory::TIME) {
                __SFC.insert(std::make_pair("DATE_AND_TIME_TO_TIME",
                    std::make_unique<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

        }

        
    }

    __SFC.insert(std::make_pair("ABS", 
        std::make_unique<ABS>(new ABS(AssociatedResource))));

    __SFC.insert(std::make_pair("ACOS", 
        std::make_unique<ACOS>(new ACOS(AssociatedResource))));

    __SFC.insert(std::make_pair("ASIN", 
        std::make_unique<ASIN>(new ASIN(AssociatedResource))));

    __SFC.insert(std::make_pair("ATAN", 
        std::make_unique<ATAN>(new ATAN(AssociatedResource))));

    __SFC.insert(std::make_pair("COS", 
        std::make_unique<COS>(new COS(AssociatedResource))));

    __SFC.insert(std::make_pair("EXP", 
        std::make_unique<EXP>(new EXP(AssociatedResource))));

    __SFC.insert(std::make_pair("LIMIT", 
        std::make_unique<LIMIT>(new LIMIT(AssociatedResource))));

    __SFC.insert(std::make_pair("LN", 
        std::make_unique<LN>(new LN(AssociatedResource))));

    __SFC.insert(std::make_pair("LOG", 
        std::make_unique<LOG>(new LOG(AssociatedResource))));

    __SFC.insert(std::make_pair("MAX", 
        std::make_unique<Max>(new Max(AssociatedResource))));

    __SFC.insert(std::make_pair("MIN", 
        std::make_unique<Min>(new Min(AssociatedResource))));

    __SFC.insert(std::make_pair("MUX", 
        std::make_unique<MUX>(new MUX(AssociatedResource))));

    __SFC.insert(std::make_pair("SEL", 
        std::make_unique<SEL>(new SEL(AssociatedResource))));

    __SFC.insert(std::make_pair("SIN", 
        std::make_unique<SIN>(new SIN(AssociatedResource))));

    __SFC.insert(std::make_pair("SQRT", 
        std::make_unique<SQRT>(new SQRT(AssociatedResource))));

    __SFC.insert(std::make_pair("TAN", 
        std::make_unique<TAN>(new TAN(AssociatedResource))));
}

SFC* SFCRegistry::GetSFC(string SFCName) {
    auto got = __SFC.find(SFCName);

    if (got == __SFC.end())
        return nullptr; // Unknown SFC
    else {
        return got->second.get();
    }
}