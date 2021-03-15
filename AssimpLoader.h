#ifndef _pluginUFE_AssimpLoader_AssimpLoader_h_
#define _pluginUFE_AssimpLoader_AssimpLoader_h_

#include <UFEngine/UFEngine.h>
#include <UFEngine/plugin/OpenGLRenderer/OpenGLRenderer.h>
#include <plugin/assimp/assimp.h>

namespace Upp{
	
class AssimpLoader{
	public:
		static inline unsigned int pFlags;
		
		static bool LoadModel(SGLRenderer& renderer, const String& modelName, const String& filePath){
			if(FileExists(filePath)){
				Assimp::Importer Importer;
				const aiScene* pScene = Importer.ReadFile(filePath.ToStd().c_str(),pFlags);
				if(pScene){
					try{
						Model& model = renderer.GetModelManager().Create(modelName);
						
						int textureStartingIterator = renderer.GetTextureManager().GetCount() -1;
						int textureCount = pScene->mNumMaterials;
						
						// Initialises the scene meshes, one by one
					    for (unsigned int i = 0 ; i < pScene->mNumMeshes ; i++) {
					        const aiMesh* paiMesh = pScene->mMeshes[i];
					        InitMesh(model,i, paiMesh);
					    }
					    model.Load();
						return true;
					}catch(Exc& exception){
						throw exception;
					}
				}else{
					throw Exc("Assimp Loader : Model can't be load, " + Format("Error parsing '%s': '%s'\n", filePath,   String(Importer.GetErrorString())  )   );
				}
			}
			throw Exc("Assimp Loader : Model can't be load, filepath is incorrect");
		}
		
	private:
		
		static void InitMesh(Model& model, int position, const aiMesh* mesh){
			
			struct Vertex : public Moveable<Vertex>{
				float pos[3];
				float normals[3];
				float texCoords[2];
				
				Vertex(float x, float y, float z, float n1 , float n2, float n3, float tc1 , float tc2){
					pos[0] = x; pos[1] = y; pos[2] = z;
					normals[0] = n1; normals[1] = n2; normals[2] = n3;
					texCoords[0] = tc1; texCoords[1] = tc2;
				}
			};
			
			Vector<Vertex> dummyVertices;
			Vector<int> dummyIndices;
			
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		
		    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
		        const aiVector3D* pPos = &(mesh->mVertices[i]);
		        const aiVector3D* pNormal = (&(mesh->mNormals[i]))? &(mesh->mNormals[i]) : &Zero3D;
		        
		        glm::vec3 norm(pNormal->x , pNormal->y , pNormal->z);
		        norm = glm::abs(norm) * -1.0f;
		        
		        const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

				//dummyVertices << Vertex( pPos->x , pPos->y , pPos->z , pNormal->x , pNormal->y , pNormal->z , pTexCoord->x , pTexCoord->y);
				dummyVertices << Vertex( pPos->x , pPos->y , pPos->z , norm.x , norm.y , norm.z , pTexCoord->x , pTexCoord->y);
		    }
			
			for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
		        const aiFace& Face = mesh->mFaces[i];
		        ASSERT_(Face.mNumIndices == 3, "Face in Assimp strucure do not contain 3 points !");
		        dummyIndices << Face.mIndices[0] << Face.mIndices[1] << Face.mIndices[2];
		    }
		    
		    Vector<float> datas;
			for(int i = 0 ; i < dummyIndices.GetCount(); i++){
				Vertex& vertex = dummyVertices[dummyIndices[i]];
				datas <<  vertex.pos[0] << vertex.pos[1] << vertex.pos[2] << vertex.normals[0] << vertex.normals[1] << vertex.normals[2] << vertex.texCoords[0] << vertex.texCoords[1];
			}
			model.AddVertices(datas, dummyIndices.GetCount());
		}
			
		static void LoadDefaultpFlag(){
			pFlags = aiProcess_JoinIdenticalVertices |// join identical vertices/ optimize indexing
			aiProcess_ValidateDataStructure |	// perform a full validation of the loader's output
			aiProcess_ImproveCacheLocality |	// improve the cache locality of the output vertices
			aiProcess_RemoveRedundantMaterials |// remove redundant materials
			aiProcess_GenUVCoords |				// convert spherical, cylindrical, box and planar mapping to proper UVs
			aiProcess_TransformUVCoords |		// pre-process UV transformations (scaling, translation ...)
			aiProcess_FindInstances |			// search for instanced meshes and remove them by references to one master
			aiProcess_LimitBoneWeights |		// limit bone weights to 4 per vertex
			aiProcess_OptimizeMeshes |			// join small meshes, if possible;
			aiProcess_PreTransformVertices |
			aiProcess_GenSmoothNormals |		// generate smooth normal vectors if not existing
			aiProcess_SplitLargeMeshes |		// split large, unrenderable meshes into sub-meshes
			aiProcess_Triangulate |				// triangulate polygons with more than 3 edges
			aiProcess_ConvertToLeftHanded |		// convert everything to D3D left handed space
			aiProcess_SortByPType;
		}

};
	



};


#endif
