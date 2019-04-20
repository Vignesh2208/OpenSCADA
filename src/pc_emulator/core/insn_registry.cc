#include <assert.h>
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

InsnRegistry::InsnRegistry(PCResourceImpl * AssociatedResource) {
    __InsnRegistry.insert(
        std::make_pair("LD",
        std::unique_ptr<Insn>(new LD_Insn(AssociatedResource, false))
    ));

     __InsnRegistry.insert(
        std::make_pair("LDN",
        std::unique_ptr<Insn>(new LD_Insn(AssociatedResource, true))
    ));
    
    __InsnRegistry.insert(
        std::make_pair("ST",
        std::unique_ptr<Insn>(new ST_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("STN",
        std::unique_ptr<Insn>(new ST_Insn(AssociatedResource, true))
    ));

    __InsnRegistry.insert(
        std::make_pair("ADD",
        std::unique_ptr<Insn>(new ADD_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("DIV",
        std::unique_ptr<Insn>(new DIV_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("EQ",
        std::unique_ptr<Insn>(new EQ_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("GE",
        std::unique_ptr<Insn>(new GE_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("GT",
        std::unique_ptr<Insn>(new GT_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("LE",
        std::unique_ptr<Insn>(new LE_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("LT",
        std::unique_ptr<Insn>(new LT_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("MOD",
        std::unique_ptr<Insn>(new MOD_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("MUL",
        std::unique_ptr<Insn>(new MUL_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("NE",
        std::unique_ptr<Insn>(new NE_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("NOT",
        std::unique_ptr<Insn>(new NOT_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("OR",
        std::unique_ptr<Insn>(new OR_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("ORN",
        std::unique_ptr<Insn>(new OR_Insn(AssociatedResource, true))
    ));

    __InsnRegistry.insert(
        std::make_pair("AND",
        std::unique_ptr<Insn>(new AND_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("ANDN",
        std::unique_ptr<Insn>(new AND_Insn(AssociatedResource, true))
    ));

    __InsnRegistry.insert(
        std::make_pair("SUB",
        std::unique_ptr<Insn>(new SUB_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("SHL",
        std::unique_ptr<Insn>(new SHL_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("SHR",
        std::unique_ptr<Insn>(new SHR_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("XOR",
        std::unique_ptr<Insn>(new XOR_Insn(AssociatedResource, false))
    ));

    __InsnRegistry.insert(
        std::make_pair("XORN",
        std::unique_ptr<Insn>(new XOR_Insn(AssociatedResource, true))
    ));
};

Insn* InsnRegistry::GetInsn(string InsnName) {
    auto got = __InsnRegistry.find(InsnName);

    if (got == __InsnRegistry.end())
        return nullptr; // Unknown Insn
    else {
        return got->second.get();
    }
}