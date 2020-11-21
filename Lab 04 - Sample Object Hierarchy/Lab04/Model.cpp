#include "Model.h"

Model::Model(const char* file_name) {
	mesh_data = load_mesh(file_name);
}

ModelData Model::load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}
	aiReleaseImport(scene);
	return modelData;
}

void Model::generateVAO(GLuint shaderProgramID) {

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.
						
	glGenVertexArrays(1, &vao);																								//unsigned int vao = 0;																								// a pointer to the vao
	glBindVertexArray(vao);																								// bind the vao

	unsigned int vp_vbo = 0;																							// a pointer to vertex position vbo
	glGenBuffers(1, &vp_vbo);																							// generate buffer for the vertex position vbo
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);																				// bind a buffer to the vertex position vbo
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);		// load data to the vertex position vbo

	GLuint vertex_position_location_in_shader = glGetAttribLocation(shaderProgramID, "vertex_position");														// a pointer to vertex position in the shader
	glEnableVertexAttribArray(vertex_position_location_in_shader);																					// point at the vertex position in the shader
	glVertexAttribPointer(vertex_position_location_in_shader, 3, GL_FLOAT, GL_FALSE, 0, NULL);														// store vertex position location in the vao

	unsigned int vn_vbo = 0;																							// a pointer to the vertex normal vbo
	glGenBuffers(1, &vn_vbo);																							// generate buffer for the vertex normal vbo
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);																				// bind a buffer to the vertex normal vbo
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);		// load data to the vertex normal vbo

	GLuint vertex_normal_location_in_shader = glGetAttribLocation(shaderProgramID, "vertex_normal");														// a pointer to vertex normal in the shader
	glEnableVertexAttribArray(vertex_normal_location_in_shader);																					// point at the vertex normal in the shader
	glVertexAttribPointer(vertex_normal_location_in_shader, 3, GL_FLOAT, GL_FALSE, 0, NULL);														// store vertex normal location in the vao

	unsigned int vt_vbo = 0;
	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mTextureCoords[0], GL_STATIC_DRAW);

	GLuint vertex_texture_location_in_shader = glGetAttribLocation(shaderProgramID, "vertex_texture");														// a pointer to the vertex texture in the shader
	glEnableVertexAttribArray(vertex_texture_location_in_shader);
	glVertexAttribPointer(vertex_texture_location_in_shader, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}
