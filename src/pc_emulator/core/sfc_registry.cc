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
                <= DataTypeCategory::TIME
                && ElementaryDatatypes[j]->__DataTypeCategory 
                <= DataTypeCategory::LREAL) {

                string conv_block_name 
                = ElementaryDatatypes[i]->__DataTypeName + "_TO_"
                  + ElementaryDatatypes[j]->__DataTypeName;

                //std::cout << "CREATING SFC: " << conv_block_name << std::endl;
                __SFC.insert(std::make_pair(conv_block_name,
                    std::unique_ptr<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

            if (ElementaryDatatypes[i]->__DataTypeCategory 
                == DataTypeCategory::DATE_AND_TIME
                && ElementaryDatatypes[j]->__DataTypeCategory 
                == DataTypeCategory::DATE) {
                __SFC.insert(std::make_pair("DATE_AND_TIME_TO_DATE",
                    std::unique_ptr<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

            if (ElementaryDatatypes[i]->__DataTypeCategory 
                == DataTypeCategory::DATE_AND_TIME
                && ElementaryDatatypes[j]->__DataTypeCategory 
                == DataTypeCategory::TIME_OF_DAY) {
                __SFC.insert(std::make_pair("DATE_AND_TIME_TO_TIME_OF_DAY",
                    std::unique_ptr<ANY_TO_ANY>(new ANY_TO_ANY(
                        AssociatedResource, ElementaryDatatypes[j],
                        ElementaryDatatypes[i]
                    ))));
            }

        }

        
    }

    __SFC.insert(std::make_pair("ABS", 
        std::unique_ptr<ABS>(new ABS(AssociatedResource))));

    __SFC.insert(std::make_pair("ACOS", 
        std::unique_ptr<ACOS>(new ACOS(AssociatedResource))));

    __SFC.insert(std::make_pair("ASIN", 
        std::unique_ptr<ASIN>(new ASIN(AssociatedResource))));

    __SFC.insert(std::make_pair("ATAN", 
        std::unique_ptr<ATAN>(new ATAN(AssociatedResource))));

    __SFC.insert(std::make_pair("COS", 
        std::unique_ptr<COS>(new COS(AssociatedResource))));

    __SFC.insert(std::make_pair("EXP", 
        std::unique_ptr<EXP>(new EXP(AssociatedResource))));

    __SFC.insert(std::make_pair("GTOD", 
        std::unique_ptr<GTOD>(new GTOD(AssociatedResource))));

    __SFC.insert(std::make_pair("LIMIT", 
        std::unique_ptr<LIMIT>(new LIMIT(AssociatedResource))));

    __SFC.insert(std::make_pair("LN", 
        std::unique_ptr<LN>(new LN(AssociatedResource))));

    __SFC.insert(std::make_pair("LOG", 
        std::unique_ptr<LOG>(new LOG(AssociatedResource))));

    __SFC.insert(std::make_pair("MAX", 
        std::unique_ptr<Max>(new Max(AssociatedResource))));

    __SFC.insert(std::make_pair("MIN", 
        std::unique_ptr<Min>(new Min(AssociatedResource))));

    __SFC.insert(std::make_pair("MUX", 
        std::unique_ptr<MUX>(new MUX(AssociatedResource))));

    __SFC.insert(std::make_pair("SEL", 
        std::unique_ptr<SEL>(new SEL(AssociatedResource))));

    __SFC.insert(std::make_pair("SIN", 
        std::unique_ptr<SIN>(new SIN(AssociatedResource))));

    __SFC.insert(std::make_pair("SQRT", 
        std::unique_ptr<SQRT>(new SQRT(AssociatedResource))));

    __SFC.insert(std::make_pair("TAN", 
        std::unique_ptr<TAN>(new TAN(AssociatedResource))));
}

SFC* SFCRegistry::GetSFC(string SFCName) {
    auto got = __SFC.find(SFCName);

    if (got == __SFC.end())
        return nullptr; // Unknown SFC
    else {
        return got->second.get();
    }
}