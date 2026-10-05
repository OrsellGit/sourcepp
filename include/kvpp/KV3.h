#pragma once

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <sourcepp/Math.h>

namespace kvpp {

namespace KV3Value {

using UUID = std::array<std::byte, 16>;

struct Element {
	int32_t index;
	std::string externalUUID;

	static constexpr int32_t NULL_INDEX = -1;
	static constexpr int32_t EXTERNAL_INDEX = -2;
};

using ByteArray = std::vector<std::byte>;

struct Time {
	float seconds;
};

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

using Vector2 = sourcepp::math::Vec2f;

using Vector3 = sourcepp::math::Vec3f;

using Vector4 = sourcepp::math::Vec4f;

struct EulerAngles : sourcepp::math::EulerAngles {};

struct Quaternion : sourcepp::math::Quat {};

using Matrix4x4 = sourcepp::math::Mat4x4f;

// TODO: This needs to contain the standard set of
enum class ID : uint8_t {
	INVALID = 0,

	VALUE_START,
	ELEMENT = VALUE_START,
	INT32,
	FLOAT,
	BOOL,
	STRING,
	BYTEARRAY,
	UUID,
	TIME,
	COLOR,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	EULER_ANGLES,
	QUATERNION,
	MATRIX_4X4,
	UINT64,
	UINT8,
	VALUE_END = UINT8,

	ARRAY_START,
	ARRAY_ELEMENT = ARRAY_START,
	ARRAY_INT32,
	ARRAY_FLOAT,
	ARRAY_BOOL,
	ARRAY_STRING,
	ARRAY_BYTEARRAY,
	ARRAY_UUID,
	ARRAY_TIME,
	ARRAY_COLOR,
	ARRAY_VECTOR2,
	ARRAY_VECTOR3,
	ARRAY_VECTOR4,
	ARRAY_EULER_ANGLES,
	ARRAY_QUATERNION,
	ARRAY_MATRIX_4X4,
	ARRAY_UINT64,
	ARRAY_UINT8,
	ARRAY_END = ARRAY_UINT8,

	TABLE_START,
	TABLE_ELEMENT = TABLE_START,
	TABLE_INT32,
	TABLE_FLOAT,
	TABLE_BOOL,
	TABLE_STRING,
	TABLE_BYTEARRAY,
	TABLE_UUID,
	TABLE_TIME,
	TABLE_COLOR,
	TABLE_VECTOR2,
	TABLE_VECTOR3,
	TABLE_VECTOR4,
	TABLE_EULER_ANGLES,
	TABLE_QUATERNION,
	TABLE_MATRIX_4X4,
	TABLE_UINT64,
	TABLE_UINT8,
	TABLE_END = TABLE_UINT8,
};

[[nodiscard]] constexpr std::byte encodeID(const ID id) {
	auto out = ID::INVALID;
	switch (id) {
		case ID::INVALID:            break;
		case ID::ELEMENT:            out = ID::ELEMENT; break;
		case ID::INT32:              out = ID::INT32; break;
		case ID::FLOAT:              out = ID::FLOAT; break;
		case ID::BOOL:               out = ID::BOOL; break;
		case ID::STRING:             out = ID::STRING; break;
		case ID::BYTEARRAY:          out = ID::BYTEARRAY; break;
		case ID::UUID:               out = ID::UUID; break;
		case ID::TIME:               out = ID::STRING; break;
		case ID::COLOR:              out = ID::COLOR; break;
		case ID::VECTOR2:            out = ID::VECTOR2; break;
		case ID::VECTOR3:            out = ID::VECTOR3; break;
		case ID::VECTOR4:            out = ID::VECTOR4; break;
		case ID::EULER_ANGLES:       out = ID::EULER_ANGLES; break;
		case ID::QUATERNION:         out = ID::QUATERNION; break;
		case ID::MATRIX_4X4:         out = ID::MATRIX_4X4; break;
		case ID::UINT64:
		case ID::UINT8:              out = ID::STRING; break;

		case ID::ARRAY_ELEMENT:      out = ID::ARRAY_ELEMENT; break;
		case ID::ARRAY_INT32:        out = ID::ARRAY_INT32; break;
		case ID::ARRAY_FLOAT:        out = ID::ARRAY_FLOAT; break;
		case ID::ARRAY_BOOL:         out = ID::ARRAY_BOOL; break;
		case ID::ARRAY_STRING:       out = ID::ARRAY_STRING; break;
		case ID::ARRAY_BYTEARRAY:    out = ID::ARRAY_BYTEARRAY; break;
		case ID::ARRAY_UUID:         out = ID::ARRAY_UUID; break;
		case ID::ARRAY_TIME:         out = ID::ARRAY_STRING; break;
		case ID::ARRAY_COLOR:        out = ID::ARRAY_COLOR; break;
		case ID::ARRAY_VECTOR2:      out = ID::ARRAY_VECTOR2; break;
		case ID::ARRAY_VECTOR3:      out = ID::ARRAY_VECTOR3; break;
		case ID::ARRAY_VECTOR4:      out = ID::ARRAY_VECTOR4; break;
		case ID::ARRAY_EULER_ANGLES: out = ID::ARRAY_EULER_ANGLES; break;
		case ID::ARRAY_QUATERNION:   out = ID::ARRAY_QUATERNION; break;
		case ID::ARRAY_MATRIX_4X4:   out = ID::ARRAY_MATRIX_4X4; break;
		case ID::ARRAY_UINT64:
		case ID::ARRAY_UINT8:        out = ID::TABLE_STRING; break;

		case ID::TABLE_ELEMENT:      out = ID::TABLE_ELEMENT; break;
		case ID::TABLE_INT32:        out = ID::TABLE_INT32; break;
		case ID::TABLE_FLOAT:        out = ID::TABLE_FLOAT; break;
		case ID::TABLE_BOOL:         out = ID::TABLE_BOOL; break;
		case ID::TABLE_STRING:       out = ID::TABLE_STRING; break;
		case ID::TABLE_BYTEARRAY:    out = ID::TABLE_BYTEARRAY; break;
		case ID::TABLE_UUID:         out = ID::TABLE_UUID; break;
		case ID::TABLE_TIME:         out = ID::TABLE_STRING; break;
		case ID::TABLE_COLOR:        out = ID::TABLE_COLOR; break;
		case ID::TABLE_VECTOR2:      out = ID::TABLE_VECTOR2; break;
		case ID::TABLE_VECTOR3:      out = ID::TABLE_VECTOR3; break;
		case ID::TABLE_VECTOR4:      out = ID::TABLE_VECTOR4; break;
		case ID::TABLE_EULER_ANGLES: out = ID::TABLE_EULER_ANGLES; break;
		case ID::TABLE_QUATERNION:   out = ID::TABLE_QUATERNION; break;
		case ID::TABLE_MATRIX_4X4:   out = ID::TABLE_MATRIX_4X4; break;
		case ID::TABLE_UINT64:
		case ID::TABLE_UINT8:        out = ID::TABLE_STRING; break;
	}
	return static_cast<std::byte>(out);
}

using Generic = std::variant<
	std::monostate,

	Element,
	int32_t,
	float,
	bool,
	std::string,
	std::vector<std::byte>,
	UUID,
	Time,
	Color,
	Vector2,
	Vector3,
	Vector4,
	EulerAngles,
	Quaternion,
	Matrix4x4,
	uint64_t,
	uint8_t,

	std::vector<Element>,
	std::vector<int32_t>,
	std::vector<float>,
	std::vector<bool>,
	std::vector<std::string>,
	std::vector<std::vector<std::byte>>,
	std::vector<UUID>,
	std::vector<Time>,
	std::vector<Color>,
	std::vector<Vector2>,
	std::vector<Vector3>,
	std::vector<Vector4>,
	std::vector<EulerAngles>,
	std::vector<Quaternion>,
	std::vector<Matrix4x4>,
	std::vector<uint64_t>,
	std::vector<uint8_t>
>;

[[nodiscard]] constexpr ID arrayIDToInnerID(ID id) {
	if (id >= ID::ARRAY_START) {
		return static_cast<ID>(static_cast<uint8_t>(id) - static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

[[nodiscard]] constexpr ID innerIDToArrayID(ID id) {
	if (id <= ID::VALUE_END) {
		return static_cast<ID>(static_cast<uint8_t>(id) + static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

[[nodiscard]] std::string idToString(ID id);

// NOLINTNEXTLINE(*-no-recursion)
[[nodiscard]] constexpr ID stringToID(const std::string_view id) {
	using enum ID;
	if (id == "element")    return ELEMENT;
	if (id == "int")        return INT32;
	if (id == "float")      return FLOAT;
	if (id == "bool")       return BOOL;
	if (id == "string")     return STRING;
	if (id == "binary")     return BYTEARRAY;
	if (id == "elementid")  return UUID;
	if (id == "time")       return TIME;
	if (id == "color")      return COLOR;
	if (id == "vector2")    return VECTOR2;
	if (id == "vector3")    return VECTOR3;
	if (id == "vector4")    return VECTOR4;
	if (id == "angle")      return EULER_ANGLES;
	if (id == "quaternion") return QUATERNION;
	if (id == "matrix")     return MATRIX_4X4;
	if (id == "uint64")     return UINT64;
	if (id == "uint8")      return UINT8;
	if (id.ends_with("_array")) {
		return innerIDToArrayID(stringToID(id.substr(0, id.length() - 6)));
	}
	return INVALID;
}

} // namespace KV3Value

class KV3Attribute {
	friend class KV3Element;
	friend class KV3;

public:
	/// Check if the given attribute is invalid
	[[nodiscard]] bool isInvalid() const;

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] std::string_view getKey() const;

	void setKey(std::string key_);

	[[nodiscard]] KV3Value::ID getValueType() const;

	[[nodiscard]] bool isValueArray() const;

	[[nodiscard]] const KV3Value::Generic& getValue() const;

	template<typename T>
	[[nodiscard]] T getValue() const {
		return std::get<T>(this->value);
	}

	[[nodiscard]] std::string getValueString() const;

	/// Set the value associated with the attribute
	void setValue(KV3Value::Generic value_);

	/// Set the value associated with the attribute
	KV3Attribute& operator=(KV3Value::Generic value_);

protected:
	KV3Attribute() = default;

	std::string key;
	KV3Value::Generic value;
};

class KV3Element {
	friend class KV3;

public:
	[[nodiscard]] explicit operator bool() const;

	/// Get the C++ type the element maps to
	[[nodiscard]] std::string_view getType() const;

	/// Set the C++ type the element maps to
	void setType(std::string type_);

	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const;

	/// Set the key associated with the element
	void setKey(std::string key_);

	// Get the UUID associated with this element
	[[nodiscard]] const KV3Value::UUID& getUUID() const;

	// Set the UUID associated with this element
	void setUUID(const KV3Value::UUID& uuid_);

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasAttribute(std::string_view attributeKey) const;

	/// Add an attribute to the element
	KV3Attribute& addAttribute(std::string key_, KV3Value::Generic value_ = {});

	/// Get the number of child attributes
	[[nodiscard]] uint64_t getAttributeCount() const;

	/// Get the number of child attributes with the given key
	[[nodiscard]] uint64_t getAttributeCount(std::string_view childKey) const;

	/// Get the child attributes of the element
	[[nodiscard]] const std::vector<KV3Attribute>& getAttributes() const;

	/// Get the child attributes of the element
	[[nodiscard]] std::vector<KV3Attribute>& getAttributes();

	using iterator = std::vector<KV3Attribute>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->attributes.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->attributes.end();
	}

	using const_iterator = std::vector<KV3Attribute>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->attributes.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->attributes.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->attributes.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->attributes.cend();
	}

	/// Get the attribute of the element at the given index
	[[nodiscard]] const KV3Attribute& operator[](unsigned int n) const;

	/// Get the attribute of the element at the given index
	[[nodiscard]] KV3Attribute& operator[](unsigned int n);

	/// Get the first attribute of the element with the given key
	[[nodiscard]] const KV3Attribute& operator[](std::string_view attributeKey) const;

	/// Get the first attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Attribute& operator[](std::string_view attributeKey);

	/// Get the first attribute of the element with the given key
	[[nodiscard]] const KV3Attribute& operator()(std::string_view attributeKey) const;

	/// Get the first attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Attribute& operator()(std::string_view attributeKey);

	/// Get the nth attribute of the element with the given key
	[[nodiscard]] const KV3Attribute& operator()(std::string_view attributeKey, unsigned int n) const;

	/// Get the nth attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Attribute& operator()(std::string_view attributeKey, unsigned int n);

	/// Remove an attribute from the element.
	void removeAttribute(unsigned int n);

	/// Remove an attribute from the element with the given key. -1 means all children with the given key
	void removeAttribute(std::string_view attributeKey, int n = -1);

	static const KV3Attribute& getInvalidAttribute();

protected:
	KV3Element() = default;

	std::string type;
	std::string key;
	KV3Value::UUID uuid{};
	std::vector<KV3Attribute> attributes;
};

class KV3 {
public:
	enum Encoding {
		ENCODING_INVALID,
		ENCODING_KEYVALUES3,
		ENCODING_BINARY,
		ENCODING_BINARY_COMPRESSED,
	};

	enum Formats {
		FORMAT_INVALID,
		FORMAT_GENERIC
	};

	KV3(Encoding encodingType_, int encodingVersion_, std::string formatType_, int formatVersion_);

	explicit KV3(std::span<const std::byte> kv3Data);

	explicit KV3(std::string_view kv3Data);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] Encoding getEncodingType() const;

	void setEncodingType(Encoding encodingType_);

	[[nodiscard]] bool doesEncodingTypeHaveUnicodePrefix() const;

	void shouldEncodingTypeHaveUnicodePrefix(bool encodingTypeHasUnicodePrefix_);

	[[nodiscard]] int getEncodingVersion() const;

	void setEncodingVersion(int encodingVersion_);

	[[nodiscard]] std::string_view getFormatType() const;

	void setFormatType(std::string formatType_);

	[[nodiscard]] int getFormatVersion() const;

	void setFormatVersion(int formatVersion_);

	KV3Element& addPrefixAttributeContainer();

	/// Get the number of prefix attributes
	[[nodiscard]] uint64_t getPrefixAttributeContainerCount() const;

	[[nodiscard]] const std::vector<KV3Element>& getPrefixAttributeContainers() const;

	[[nodiscard]] std::vector<KV3Element>& getPrefixAttributeContainers();

	void removePrefixAttributeContainer(unsigned int n);

	/// Check if the element list has one or more elements with the given name
	[[nodiscard]] bool hasElement(std::string_view key) const;

	/// Add an element to the element list
	KV3Element& addElement(std::string type, std::string key);

	/// Get the number of elements
	[[nodiscard]] uint64_t getElementCount() const;

	/// Get the number of elements with the given key
	[[nodiscard]] uint64_t getElementCount(std::string_view key) const;

	[[nodiscard]] const std::vector<KV3Element>& getElements() const;

	[[nodiscard]] std::vector<KV3Element>& getElements();

	using iterator = std::vector<KV3Element>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->elements.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->elements.end();
	}

	using const_iterator = std::vector<KV3Element>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->elements.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->elements.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->elements.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->elements.cend();
	}

	/// Get the element in the element list at the given index
	[[nodiscard]] const KV3Element& operator[](unsigned int n) const;

	/// Get the element in the element list at the given index
	[[nodiscard]] KV3Element& operator[](unsigned int n);

	/// Get the first element in the element list with the given key
	[[nodiscard]] const KV3Element& operator[](std::string_view key) const;

	/// Get the first element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Element& operator[](std::string_view key);

	/// Get the first element in the element list with the given key
	[[nodiscard]] const KV3Element& operator()(std::string_view key) const;

	/// Get the first element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Element& operator()(std::string_view key);

	/// Get the nth element in the element list with the given key
	[[nodiscard]] const KV3Element& operator()(std::string_view key, unsigned int n) const;

	/// Get the nth element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV3Element& operator()(std::string_view key, unsigned int n);

	/// Remove an element from the element list and update all element references
	void removeElement(unsigned int n);

	[[nodiscard]] std::vector<std::byte> bake() const;

	void bake(const std::filesystem::path& kv3Path) const;

	// TODO: ENCODING VERSION NEEDS TO BE FIGURED OUT WITH THE UUID!
	[[nodiscard]] static constexpr bool isEncodingVersionValid(const Encoding encodingType, const int encodingVersion) {
		switch (encodingType) {
			case ENCODING_INVALID:
				break;
			case ENCODING_BINARY:
			case ENCODING_BINARY_COMPRESSED:
				return (encodingVersion >= 1 && encodingVersion <= 5) || encodingVersion == 9;
			case ENCODING_KEYVALUES3:
				return encodingVersion >= 1 && encodingVersion <= 4;
		}
		return false;
	}

	[[nodiscard]] static KV3Value::UUID createRandomUUID();

	[[nodiscard]] static const KV3Element& getInvalidElement();

protected:
	Encoding encodingType = ENCODING_INVALID;
	bool encodingTypeHasUnicodePrefix = false;
	int encodingVersion = -1;



	Formats formatType_ = FORMAT_INVALID;
	std::string formatType;
	int formatVersion = -1;

	std::vector<KV3Element> prefixAttributeContainers;
	std::vector<KV3Element> elements;
};

namespace literals {

KV3 operator""_kv3(const char* str, std::size_t len);

} // namespace literals

} // namespace kvpp
