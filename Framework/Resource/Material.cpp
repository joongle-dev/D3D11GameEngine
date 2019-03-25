#include "Framework.h"
#include "Material.h"

Material::Material(Context * context) :
	Resource<Material>(context)
{
}

void Material::LoadFromFile(std::string & path)
{
}
