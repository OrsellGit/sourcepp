// ReSharper disable CppRedundantQualifier

#include <kvpp/KV3.h>

#include <format>
#include <limits>
#include <random>
#include <sstream>
#include <utility>

#include <BufferStream.h>
#include <iomanip>
#include <sourcepp/FS.h>

using namespace kvpp;
using namespace sourcepp;

// NOLINTNEXTLINE(*-no-recursion)
std::string KV3Value::idToString(const ID id) {
	switch (id) {
		case ID::INVALID: break;
		case ID::ELEMENT:      return "element";
		case ID::INT32:        return "int";
		case ID::FLOAT:        return "float";
		case ID::BOOL:         return "bool";
		case ID::STRING:       return "string";
		case ID::BYTEARRAY:    return "binary";
		case ID::UUID:         return "elementid";
		case ID::TIME:         return "time";
		case ID::COLOR:        return "color";
		case ID::VECTOR2:      return "vector2";
		case ID::VECTOR3:      return "vector3";
		case ID::VECTOR4:      return "vector4";
		case ID::EULER_ANGLES: return "angle";
		case ID::QUATERNION:   return "quaternion";
		case ID::MATRIX_4X4:   return "matrix";
		case ID::UINT64:       return "uint64";
		case ID::UINT8:        return "uint8";
		case ID::ARRAY_ELEMENT:
		case ID::ARRAY_INT32:
		case ID::ARRAY_FLOAT:
		case ID::ARRAY_BOOL:
		case ID::ARRAY_STRING:
		case ID::ARRAY_BYTEARRAY:
		case ID::ARRAY_UUID:
		case ID::ARRAY_TIME:
		case ID::ARRAY_COLOR:
		case ID::ARRAY_VECTOR2:
		case ID::ARRAY_VECTOR3:
		case ID::ARRAY_VECTOR4:
		case ID::ARRAY_EULER_ANGLES:
		case ID::ARRAY_QUATERNION:
		case ID::ARRAY_MATRIX_4X4:
		case ID::ARRAY_UINT64:
		case ID::ARRAY_UINT8:
			return idToString(arrayIDToInnerID(id)) + "_array";
	}
	return "invalid";
}

bool KV3Attribute::isInvalid() const {
	return static_cast<KV3Value::ID>(this->value.index()) == KV3Value::ID::INVALID;
}

KV3Attribute::operator bool() const {
	return !this->isInvalid();
}

std::string_view KV3Attribute::getKey() const {
	return this->key;
}

void KV3Attribute::setKey(std::string key_) {
	this->key = std::move(key_);
}

KV3Value::ID KV3Attribute::getValueType() const {
	return static_cast<KV3Value::ID>(this->value.index());
}

bool KV3Attribute::isValueArray() const {
	return this->getValueType() >= KV3Value::ID::ARRAY_START;
}

const KV3Value::Generic& KV3Attribute::getValue() const {
	return this->value;
}

std::string KV3Attribute::getValueString() const {
	switch (const auto type = this->getValueType()) {
		using enum KV3Value::ID;
		case INVALID:
			return KV3Value::idToString(type);
		case ELEMENT: {
			const auto [index, stubUUID] = this->getValue<KV3Value::Element>();
			return std::format("{}{}", index == -2 ? "UUID: " : "#", index);
		}
		case INT32:
			return std::format("{}", this->getValue<int32_t>());
		case FLOAT:
			return std::format("{}", this->getValue<float>());
		case BOOL:
			return std::format("{}", static_cast<int>(this->getValue<bool>()));
		case STRING:
			return this->getValue<std::string>();
		case BYTEARRAY: {
			std::stringstream hex;
			hex << std::hex << std::setfill('0');
			for (auto byte : this->getValue<KV3Value::ByteArray>()) {
				hex << std::setw(2) << static_cast<unsigned char>(byte);
			}
			return hex.str();
		}
		case UUID: {
			std::stringstream hex;
			hex << std::hex << std::setfill('0');
			for (auto byte : this->getValue<KV3Value::UUID>()) {
				hex << std::setw(2) << static_cast<unsigned char>(byte);
			}
			return hex.str();
		}
		case TIME:
			return std::format("{}", this->getValue<KV3Value::Time>().seconds);
		case COLOR: {
			const auto [r, g, b, a] = this->getValue<KV3Value::Color>();
			return std::format("rgba({}, {}, {}, {})", r, g, b, a);
		}
		case VECTOR2: {
			const auto vec2 = this->getValue<KV3Value::Vector2>();
			return std::format("[{}, {}]", vec2[0], vec2[1]);
		}
		case VECTOR3: {
			const auto vec3 = this->getValue<KV3Value::Vector3>();
			return std::format("[{}, {}, {}]", vec3[0], vec3[1], vec3[2]);
		}
		case VECTOR4: {
			const auto vec4 = this->getValue<KV3Value::Vector4>();
			return std::format("[{}, {}, {}, {}]", vec4[0], vec4[1], vec4[2], vec4[3]);
		}
		case EULER_ANGLES: {
			const auto angles = this->getValue<KV3Value::EulerAngles>();
			return std::format("[{}, {}, {}]", angles[0], angles[1], angles[2]);
		}
		case QUATERNION: {
			const auto quat = this->getValue<KV3Value::Quaternion>();
			return std::format("[{}, {}, {}, {}]", quat[0], quat[1], quat[2], quat[3]);
		}
		case MATRIX_4X4: {
			const auto mat4 = this->getValue<KV3Value::Matrix4x4>();
			std::string out;
			for (int i = 0; i < 4; i++) {
				out += i == 0 ? '[' : ' ';
				for (int j = 0; j < 4; j++) {
					out += std::format("{}{}", mat4[i][j], j < 3 ? ", " : i < 3 ? ",\n" : "]");
				}
			}
			return out;
		}
		case UINT64:
			return std::format("{}", this->getValue<uint64_t>());
		case UINT8:
			return std::format("{}", static_cast<int>(this->getValue<uint8_t>()));
		case ARRAY_ELEMENT: {
			const auto elements = this->getValue<std::vector<KV3Value::Element>>();
			std::string out = "[";
			for (int i = 0; i < elements.size(); i++) {
				if (elements[i].index == -2) {
					out += std::format("{}UUID: {}{}", i == 0 ? "" : " ", elements[i].externalUUID, i == elements.size() - 1 ? "" : ",");
				} else {
					out += std::format("{}#{}{}", i == 0 ? "" : " ", elements[i].index, i == elements.size() - 1 ? "" : ",");
				}
			}
			return out + ']';
		}
		case ARRAY_INT32: {
			const auto ints = this->getValue<std::vector<int32_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", ints[i], i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_FLOAT: {
			const auto floats = this->getValue<std::vector<float>>();
			std::string out = "[";
			for (int i = 0; i < floats.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", floats[i], i == floats.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_BOOL: {
			const auto bools = this->getValue<std::vector<bool>>();
			std::string out = "[";
			for (int i = 0; i < bools.size(); i++) {
				// ReSharper disable once CppRedundantCastExpression
				out += std::format("{}{}{}", i == 0 ? "" : " ", static_cast<bool>(bools[i]), i == bools.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_STRING: {
			const auto strings = this->getValue<std::vector<std::string>>();
			std::string out = "[";
			for (int i = 0; i < strings.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", strings[i], i == strings.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_BYTEARRAY: {
			const auto bytearrays = this->getValue<std::vector<KV3Value::ByteArray>>();
			std::string out = "[";
			for (int i = 0; i < bytearrays.size(); i++) {
				std::stringstream hex;
				hex << std::hex << std::setfill('0');
				for (auto byte : bytearrays[i]) {
					hex << std::setw(2) << static_cast<unsigned char>(byte);
				}
				out += std::format("{}{}{}", i == 0 ? "" : " ", hex.str(), i == bytearrays.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_UUID: {
			const auto uuids = this->getValue<std::vector<KV3Value::UUID>>();
			std::string out = "[";
			for (int i = 0; i < uuids.size(); i++) {
				std::stringstream hex;
				hex << std::hex << std::setfill('0');
				for (auto byte : uuids[i]) {
					hex << std::setw(2) << static_cast<unsigned char>(byte);
				}
				out += std::format("{}{}{}", i == 0 ? "" : " ", hex.str(), i == uuids.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_TIME: {
			const auto times = this->getValue<std::vector<KV3Value::Time>>();
			std::string out = "[";
			for (int i = 0; i < times.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", times[i].seconds, i == times.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_COLOR: {
			const auto colors = this->getValue<std::vector<KV3Value::Color>>();
			std::string out = "[";
			for (int i = 0; i < colors.size(); i++) {
				out += std::format("{}rgba({}, {}, {}, {}){}", i == 0 ? "" : " ", colors[i].r, colors[i].g, colors[i].b, colors[i].a, i == colors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR2: {
			const auto vecs = this->getValue<std::vector<KV3Value::Vector2>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += std::format("{}[{}, {}]{}", i == 0 ? "" : " ", vecs[i][0], vecs[i][1], i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR3: {
			const auto vecs = this->getValue<std::vector<KV3Value::Vector3>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += std::format("{}[{}, {}, {}]{}", i == 0 ? "" : " ", vecs[i][0], vecs[i][1], vecs[i][2], i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR4: {
			const auto vecs = this->getValue<std::vector<KV3Value::Vector4>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += std::format("{}[{}, {}, {}, {}]{}", i == 0 ? "" : " ", vecs[i][0], vecs[i][1], vecs[i][2], vecs[i][3], i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_EULER_ANGLES: {
			const auto angles = this->getValue<std::vector<KV3Value::EulerAngles>>();
			std::string out = "[";
			for (int i = 0; i < angles.size(); i++) {
				out += std::format("{}[{}, {}, {}]{}", i == 0 ? "" : " ", angles[i][0], angles[i][1], angles[i][2], i == angles.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_QUATERNION: {
			const auto quats = this->getValue<std::vector<KV3Value::Quaternion>>();
			std::string out = "[";
			for (int i = 0; i < quats.size(); i++) {
				out += std::format("{}[{}, {}, {}, {}]{}", i == 0 ? "" : " ", quats[i][0], quats[i][1], quats[i][2], quats[i][3], i == quats.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_MATRIX_4X4: {
			const auto matrices = this->getValue<std::vector<KV3Value::Matrix4x4>>();
			std::string out = "[";
			for (int m = 0; m < matrices.size(); m++) {
				out += m == 0 ? "[" : " [";
				for (int i = 0; i < 4; i++) {
					out += i == 0 ? "" : "  ";
					for (int j = 0; j < 4; j++) {
						out += std::format("{}{}", matrices[m][i][j], j < 3 ? ", " : i < 3 ? ",\n" : "]");
					}
				}
				if (m < matrices.size() - 1) {
					out += ",\n";
				}
			}
			return out + ']';
		}
		case ARRAY_UINT64: {
			const auto ints = this->getValue<std::vector<uint64_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", ints[i], i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_UINT8: {
			const auto ints = this->getValue<std::vector<uint8_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += std::format("{}{}{}", i == 0 ? "" : " ", ints[i], i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
	}
	return "";
}

void KV3Attribute::setValue(KV3Value::Generic value_) {
	this->value = std::move(value_);
}

KV3Attribute& KV3Attribute::operator=(KV3Value::Generic value_) {
	this->value = std::move(value_);
	return *this;
}

KV3Element::operator bool() const {
	return !this->type.empty() || !this->key.empty() || this->uuid != KV3Value::UUID{};
}

std::string_view KV3Element::getType() const {
	return this->type;
}

void KV3Element::setType(std::string type_) {
	this->type = std::move(type_);
}

std::string_view KV3Element::getKey() const {
	return this->key;
}

void KV3Element::setKey(std::string key_) {
	this->key = std::move(key_);
}

const KV3Value::UUID& KV3Element::getUUID() const {
	return this->uuid;
}

void KV3Element::setUUID(const KV3Value::UUID& uuid_) {
	this->uuid = uuid_;
}

bool KV3Element::hasAttribute(const std::string_view attributeKey) const {
	return !this->operator[](attributeKey);
}

KV3Attribute& KV3Element::addAttribute(std::string key_, KV3Value::Generic value_) {
	KV3Attribute attr;
	attr.setKey(std::move(key_));
	attr.setValue(std::move(value_));
	this->attributes.push_back(std::move(attr));
	return this->attributes.back();
}

uint64_t KV3Element::getAttributeCount() const {
	return this->attributes.size();
}

uint64_t KV3Element::getAttributeCount(const std::string_view childKey) const {
	uint64_t count = 0;
	for (const KV3Attribute& element : this->attributes) {
		if (string::iequals(element.key, childKey)) {
			++count;
		}
	}
	return count;
}

const std::vector<KV3Attribute>& KV3Element::getAttributes() const {
	return this->attributes;
}

std::vector<KV3Attribute>& KV3Element::getAttributes() {
	return this->attributes;
}

const KV3Attribute& KV3Element::operator[](const unsigned int n) const {
	return this->attributes.at(n);
}

KV3Attribute& KV3Element::operator[](const unsigned int n) {
	return this->attributes.at(n);
}

const KV3Attribute& KV3Element::operator[](const std::string_view attributeKey) const {
	return this->operator()(attributeKey);
}

KV3Attribute& KV3Element::operator[](const std::string_view attributeKey) {
	return this->operator()(attributeKey);
}

const KV3Attribute& KV3Element::operator()(const std::string_view attributeKey) const {
	for (const auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			return attribute;
		}
	}
	return getInvalidAttribute();
}

KV3Attribute& KV3Element::operator()(const std::string_view attributeKey) {
	for (auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			return attribute;
		}
	}
	return this->addAttribute(std::string{attributeKey});
}

const KV3Attribute& KV3Element::operator()(const std::string_view attributeKey, const unsigned int n) const {
	unsigned int count = 0;
	for (const auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			if (count == n) {
				return attribute;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return getInvalidAttribute();
}

KV3Attribute& KV3Element::operator()(const std::string_view attributeKey, const unsigned int n) {
	unsigned int count = 0;
	for (auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			if (count == n) {
				return attribute;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return this->addAttribute(std::string{attributeKey});
}

void KV3Element::removeAttribute(const unsigned int n) {
	if (this->attributes.size() > n) {
		this->attributes.erase(this->attributes.begin() + n);
	}
}

void KV3Element::removeAttribute(const std::string_view attributeKey, const int n) {
	unsigned int count = 0;
	for (auto attribute = this->attributes.begin(); attribute != this->attributes.end(); ++attribute) {
		if (string::iequals(attribute->getKey(), attributeKey)) {
			if (n < 0 || count == n) {
				attribute = this->attributes.erase(attribute);
				if (count == n) {
					return;
				}
			}
			++count;
		}
	}
}

const KV3Attribute& KV3Element::getInvalidAttribute() {
	static KV3Attribute attribute;
	return attribute;
}

KV3::KV3(const Encoding encodingType_, const int encodingVersion_, std::string formatType_, const int formatVersion_)
		: encodingType{encodingType_}
		, encodingVersion{encodingVersion_}
		, formatType{std::move(formatType_)}
		, formatVersion{formatVersion_} {}

KV3::KV3(std::span<const std::byte> kv3Data) {
	BufferStreamReadOnly stream{kv3Data};

	// Header can be at most MAX_HEADER characters long followed by either a null terminator or newline
	static constexpr int MAX_FORMAT_LENGTH = 64;
	static constexpr int MAX_HEADER_LENGTH = 40 + MAX_FORMAT_LENGTH * 2;

	std::string header;
	while (header.length() < MAX_HEADER_LENGTH) {
		const char temp = stream.read<char>();
		if (temp == '\0' || temp == '\n') {
			break;
		}
		header += temp;
	}

	std::array<char, MAX_FORMAT_LENGTH> encodingTypeData{};
	std::array<char, MAX_FORMAT_LENGTH> formatTypeData{};

	static constexpr std::string_view HEADER_FORMAT = "<!-- kv3 encoding:%s:version{%40s} format:%s:version{%40s} -->";

#ifdef _WIN32
	const bool headerParsed = sscanf_s(header.c_str(), HEADER_FORMAT.data(), encodingTypeData.data(), MAX_FORMAT_LENGTH, &this->encodingVersion, formatTypeData.data(), MAX_FORMAT_LENGTH, &this->formatVersion);
#else
	const bool headerParsed = std::sscanf(header.c_str(), HEADER_FORMAT.data(), encodingTypeData.data(), &this->encodingVersion, formatTypeData.data(), &this->formatVersion); // NOLINT(*-err34-c)
#endif

	// KV3s can be used without their header. They are just treated as text encoding with generic format if there is no header. Assume this is a
	if (!headerParsed) {
		this->encodingType = ENCODING_KEYVALUES3;
		this->formatType = "generic"; //FORMAT_GENERIC;
	}

	this->formatType = formatTypeData.data();

	std::string_view encodingTypeStr = encodingTypeData.data();
	if (encodingTypeStr.starts_with("unicode_")) {
		this->encodingTypeHasUnicodePrefix = true;
		encodingTypeStr = encodingTypeStr.substr(8);
	}

	if (encodingTypeStr == "keyvalues3") {
		this->encodingType = ENCODING_KEYVALUES3;
	} else if (encodingTypeStr == "binary") {
		this->encodingType = ENCODING_BINARY;
	} else if (encodingTypeStr == "binary_bc") {
		this->encodingType = ENCODING_BINARY_COMPRESSED;
	} else {
		this->encodingType = ENCODING_INVALID;
		return;
	}

	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		this->encodingType = ENCODING_INVALID;
		return;
	}

	/*const auto readBinary = [this, &stream] {
		// Version-specific conditionals
		//const KV3Value::IDVersion attributeIDVersion = isLegacyEncoding || this->encodingVersion < 3 ? KV3Value::IDVersion::V1 : this->encodingVersion < 9 ? KV3Value::IDVersion::V2 : KV3Value::IDVersion::V3;
		const bool stringListExists = false;
		const bool elementNamesAndStringValuesAreStoredInStringList = false;
		const bool stringListLengthIsShort = this->encodingVersion < 4;
		const bool stringListIndicesAreShort = this->encodingVersion < 5;
		const bool preloadAttributeListExists = !isLegacyEncoding && this->encodingVersion > 5;

		// Eat the null terminator for the header.
		if (stream.read<char>() != 0) {
			return false;
		}

		// Helper to read a string index and get the string from the list
		std::vector<std::string> stringList;
		const auto readStringFromIndex = [stringListExists, stringListIndicesAreShort, &stringList](BufferStream& stream_) -> std::string {
			if (!stringListExists) {
				return stream_.read_string();
			}
			uint32_t index;
			if (stringListIndicesAreShort) {
				index = stream_.read<uint16_t>();
			} else {
				index = stream_.read<uint32_t>();
			}
			if (index >= stringList.size()) {
				// This is an intentional feature of the format
				return "";
			}
			return stringList.at(index);
		};

		// Helper to read a value for an attribute
		std::function<KV3Value::Generic(KV3Value::ID, bool)> readValue;
		readValue = [&stream, &readStringFromIndex, &readValue](const KV3Value::ID type, const bool useStringList) -> KV3Value::Generic {
			const auto readArrayValue = [&stream, &readValue]<typename T>(const KV3Value::ID type_) {
				std::vector<T> out;
				auto size = stream.read<uint32_t>();
				out.reserve(size);
				// TODO: This is no longer viable as there is support for multiline strings.
				for (int i = 0; i < size; i++) {
					// String arrays are always inline
					out.push_back(std::get<T>(readValue(KV3Value::arrayIDToInnerID(type_), false)));
				}
				return out;
			};
			switch (type) {
				using enum KV3Value::ID;
				case INVALID:
					return std::monostate{};
				case ELEMENT: {
					KV3Value::Element value;
					value.index = stream.read<int32_t>();
					if (value.index == -2) {
						// Read in the ASCII UUID
						value.externalUUID = stream.read_string();
					}
					return value;
				}
				case INT32:
					return stream.read<int32_t>();
				case FLOAT:
					return stream.read<float>();
				case BOOL:
					return stream.read<bool>();
				case STRING:
					return stream.read_string();
				case BYTEARRAY:
					return stream.read_bytes(stream.read<uint32_t>());
				case UUID:
					return stream.read_bytes<16>();
				case TIME:
					return KV3Value::Time{static_cast<float>(static_cast<double>(stream.read<int32_t>()) / 10000.0)};
				case COLOR: {
					KV3Value::Color value{};
					stream >> value.r >> value.g >> value.b >> value.a;
					return value;
				}
				case VECTOR2: {
					KV3Value::Vector2 value;
					stream >> value[0] >> value[1];
					return value;
				}
				case VECTOR3: {
					KV3Value::Vector3 value;
					stream >> value[0] >> value[1] >> value[2];
					return value;
				}
				case VECTOR4: {
					KV3Value::Vector4 value;
					stream >> value[0] >> value[1] >> value[2] >> value[3];
					return value;
				}
				case EULER_ANGLES: {
					KV3Value::EulerAngles value;
					stream >> value[0] >> value[1] >> value[2];
					return value;
				}
				case QUATERNION: {
					KV3Value::Quaternion value;
					stream >> value[0] >> value[1] >> value[2] >> value[3];
					return value;
				}
				case MATRIX_4X4: {
					KV3Value::Matrix4x4 value;
					stream
						>> value[0][0] >> value[0][1] >> value[0][2] >> value[0][3]
						>> value[1][0] >> value[1][1] >> value[1][2] >> value[1][3]
						>> value[2][0] >> value[2][1] >> value[2][2] >> value[2][3]
						>> value[3][0] >> value[3][1] >> value[3][2] >> value[3][3];
					return value;
				}
				case UINT64:
					return stream.read<uint64_t>();
				case UINT8:
					return stream.read<uint8_t>();
				case ARRAY_ELEMENT:
					return readArrayValue.operator()<KV3Value::Element>(type);
				case ARRAY_INT32:
					return readArrayValue.operator()<int32_t>(type);
				case ARRAY_FLOAT:
					return readArrayValue.operator()<float>(type);
				case ARRAY_BOOL:
					return readArrayValue.operator()<bool>(type);
				case ARRAY_STRING:
					return readArrayValue.operator()<std::string>(type);
				case ARRAY_BYTEARRAY:
					return readArrayValue.operator()<std::vector<std::byte>>(type);
				case ARRAY_UUID:
					return readArrayValue.operator()<KV3Value::UUID>(type);
				case ARRAY_TIME:
					return readArrayValue.operator()<KV3Value::Time>(type);
				case ARRAY_COLOR:
					return readArrayValue.operator()<KV3Value::Color>(type);
				case ARRAY_VECTOR2:
					return readArrayValue.operator()<KV3Value::Vector2>(type);
				case ARRAY_VECTOR3:
					return readArrayValue.operator()<KV3Value::Vector3>(type);
				case ARRAY_VECTOR4:
					return readArrayValue.operator()<KV3Value::Vector4>(type);
				case ARRAY_EULER_ANGLES:
				return readArrayValue.operator()<KV3Value::EulerAngles>(type);
				case ARRAY_QUATERNION:
					return readArrayValue.operator()<KV3Value::Quaternion>(type);
				case ARRAY_MATRIX_4X4:
					return readArrayValue.operator()<KV3Value::Matrix4x4>(type);
				case ARRAY_UINT64:
					return readArrayValue.operator()<uint64_t>(type);
				case ARRAY_UINT8:
					return readArrayValue.operator()<uint8_t>(type);
			}
			return std::monostate{};
		};

		// Helper to read element attributes
		const auto readAttributes = [&stream, &readStringFromIndex, &readValue](KV3Element& element, const bool useStringList) {
			const auto attributeCount = stream.read<int32_t>();
			element.attributes.reserve(attributeCount);

			for (int i = 0; i < attributeCount; i++) {
				element.attributes.push_back(KV3Attribute{});
				auto& attribute = element.attributes.back();

				attribute.setKey(readStringFromIndex(stream));

				// TODO: Originally decodeID was only applicable to DMX because its a binary type that could be easily casted to its type ID, that doesn't work for text KV3s.
				// TODO: This still is viable for binary KV3s but both binary and text need to be supported.
				//! Commented out for now to satisfy compiler!
				//auto attributeID = KV3Value::decodeID(static_cast<KV3Value::ID>(stream.read<std::byte>()));
				//attribute.setValue(readValue(attributeID, useStringList));
			}
		};

		// Preload attributes
		if (preloadAttributeListExists) {
			if (const auto preloadElementCount = stream.read<uint32_t>()) {
				this->prefixAttributeContainers.reserve(preloadElementCount);
				for (uint32_t i = 0; i < preloadElementCount; i++) {
					auto& element = this->addPrefixAttributeContainer();
					readAttributes(element, false);
				}
			}
		}

		// String list
		if (stringListExists) {
			uint32_t stringCount;
			if (stringListLengthIsShort) {
				stringCount = stream.read<uint16_t>();
			} else {
				stringCount = stream.read<uint32_t>();
			}
			stringList.reserve(stringCount);
			for (int i = 0; i < stringCount; i++) {
				stringList.push_back(stream.read_string());
			}
		}

		// Read elements
		const auto elementCount = stream.read<int32_t>();
		this->elements.reserve(elementCount);

		for (int i = 0; i < elementCount; i++) {
			this->elements.push_back(KV3Element{});
			auto& element = this->elements.back();

			element.setType(readStringFromIndex(stream));
			if (elementNamesAndStringValuesAreStoredInStringList) {
				element.setKey(readStringFromIndex(stream));
			} else {
				element.setKey(stream.read_string());
			}
			element.setUUID(stream.read_bytes<16>());
		}

		// Read element attributes
		for (auto& element : this->elements) {
			readAttributes(element, elementNamesAndStringValuesAreStoredInStringList);
		}

		return true;
	};*/

	const auto readText = [this, &stream]
	{
		return false;
	};

	bool parseSuccess = false;
	switch (this->encodingType) {
		case ENCODING_INVALID:
			break;
		case ENCODING_KEYVALUES3:
			parseSuccess = readText();
			break;
		case ENCODING_BINARY:
			parseSuccess = false; //readBinary();
		case ENCODING_BINARY_COMPRESSED:
			parseSuccess = false; //readBinaryCompressed();
			break;
	}
	if (!parseSuccess) {
		this->encodingType = ENCODING_INVALID;
		this->elements.clear();
	}
}

KV3::KV3(const std::string_view kv3Data)
		: KV3{{reinterpret_cast<const std::byte*>(kv3Data.data()), kv3Data.size()}} {}

KV3::operator bool() const {
	return this->encodingType != ENCODING_INVALID;
}

KV3::Encoding KV3::getEncodingType() const {
	return this->encodingType;
}

void KV3::setEncodingType(const Encoding encodingType_) {
	this->encodingType = encodingType_;
	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		this->encodingVersion = 1;
	}
}

bool KV3::doesEncodingTypeHaveUnicodePrefix() const {
	return this->encodingTypeHasUnicodePrefix;
}

void KV3::shouldEncodingTypeHaveUnicodePrefix(const bool encodingTypeHasUnicodePrefix_) {
	this->encodingTypeHasUnicodePrefix = encodingTypeHasUnicodePrefix_;
}

int KV3::getEncodingVersion() const {
	return this->encodingVersion;
}

void KV3::setEncodingVersion(const int encodingVersion_) {
	if (isEncodingVersionValid(this->encodingType, encodingVersion_)) {
		this->encodingVersion = encodingVersion_;
	}
}

std::string_view KV3::getFormatType() const {
	return this->formatType;
}

void KV3::setFormatType(std::string formatType_) {
	this->formatType = std::move(formatType_);
}

int KV3::getFormatVersion() const {
	return this->formatVersion;
}

void KV3::setFormatVersion(const int formatVersion_) {
	this->formatVersion = formatVersion_;
}

KV3Element& KV3::addPrefixAttributeContainer() {
	this->prefixAttributeContainers.push_back(KV3Element{});
	return this->prefixAttributeContainers.back();
}

uint64_t KV3::getPrefixAttributeContainerCount() const {
	return this->prefixAttributeContainers.size();
}

const std::vector<KV3Element>& KV3::getPrefixAttributeContainers() const {
	return this->prefixAttributeContainers;
}

std::vector<KV3Element> & KV3::getPrefixAttributeContainers() {
	return this->prefixAttributeContainers;
}

void KV3::removePrefixAttributeContainer(const unsigned int n) {
	if (this->prefixAttributeContainers.size() > n) {
		this->prefixAttributeContainers.erase(this->prefixAttributeContainers.begin() + n);
	}
}

bool KV3::hasElement(const std::string_view key) const {
	return !this->operator[](key);
}

KV3Element& KV3::addElement(std::string type, std::string key) {
	KV3Element elem;
	elem.setType(std::move(type));
	elem.setKey(std::move(key));
	elem.setUUID(createRandomUUID());
	this->elements.push_back(std::move(elem));
	return this->elements.back();
}

uint64_t KV3::getElementCount() const {
	return this->elements.size();
}

uint64_t KV3::getElementCount(const std::string_view key) const {
	uint64_t count = 0;
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			++count;
		}
	}
	return count;
}

const std::vector<KV3Element>& KV3::getElements() const {
	return this->elements;
}

std::vector<KV3Element>& KV3::getElements() {
	return this->elements;
}

const KV3Element& KV3::operator[](const unsigned int n) const {
	return this->elements.at(n);
}

KV3Element& KV3::operator[](const unsigned int n) {
	return this->elements.at(n);
}

const KV3Element& KV3::operator[](const std::string_view key) const {
	return this->operator()(key);
}

KV3Element& KV3::operator[](const std::string_view key) {
	return this->operator()(key);
}

const KV3Element& KV3::operator()(const std::string_view key) const {
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			return element;
		}
	}
	return getInvalidElement();
}

KV3Element& KV3::operator()(const std::string_view key) {
	for (auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			return element;
		}
	}
	return this->addElement("DmElement", std::string{key});
}

const KV3Element& KV3::operator()(const std::string_view key, const unsigned int n) const {
	unsigned int count = 0;
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			if (count == n) {
				return element;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return getInvalidElement();
}

KV3Element& KV3::operator()(const std::string_view key, const unsigned int n) {
	unsigned int count = 0;
	for (auto& element: this->elements) {
		if (string::iequals(element.getKey(), key)) {
			if (count == n) {
				return element;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return this->addElement("DmElement", std::string{key});
}

void KV3::removeElement(const unsigned int n) {
	if (this->elements.size() > n) {
		this->elements.erase(this->elements.begin() + n);
	}
	for (auto& element : this->elements) {
		for (auto& attribute : element.attributes) {
			if (const auto attributeType = attribute.getValueType(); attributeType == KV3Value::ID::ELEMENT) {
				auto elementRef = attribute.getValue<KV3Value::Element>();
				if (elementRef.index > n) {
					elementRef.index--;
				}
				attribute.setValue(elementRef);
			} else if (attributeType == KV3Value::ID::ARRAY_ELEMENT) {
				auto elementsRef = attribute.getValue<std::vector<KV3Value::Element>>();
				bool shouldSet = false;
				for (auto& [index, externalUUID] : elementsRef) {
					if (index > n) {
						index--;
						shouldSet = true;
					}
				}
				if (shouldSet) {
					attribute.setValue(elementsRef);
				}
			}
		}
	}
}

std::vector<std::byte> KV3::bake() const {
	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		return {};
	}

	// todo: bake
	return {};
}

void KV3::bake(const std::filesystem::path& kv3Path) const {
	fs::writeFileBuffer(kv3Path, this->bake());
}

KV3Value::UUID KV3::createRandomUUID() {
	static std::random_device random_device{};
	static std::mt19937 generator{random_device()};
	std::uniform_int_distribution<short> distribution{std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()};

	KV3Value::UUID uuid;
	for (auto& byte : uuid) {
		byte = static_cast<std::byte>(distribution(generator));
	}
	return uuid;
}

const KV3Element& KV3::getInvalidElement() {
	static KV3Element element;
	return element;
}

KV3 literals::operator ""_kv3(const char *str, std::size_t len) {
	return KV3{{str, len}};
}
