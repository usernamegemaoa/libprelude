
/* Auto-generated by the GenerateIDMEFTreePrintC package */

#include <stdio.h>
#include <time.h>

#include "idmef.h"
#include "idmef-tree-wrap.h"
#include "idmef-tree-print.h"


static int indent = 0;

static void print_indent(void)
{
	int cnt;

	for ( cnt = 0; cnt < indent; cnt++ )
		printf(" ");
}



static void print_string(prelude_string_t *string)
{
	const char *s;

	s = prelude_string_get_string(string);
	printf("%s", s ? s : "<empty>");
}


/*
static void print_int16(int16_t i)
{
	printf("%hd", i);
}
*/


static void print_uint8(uint8_t i)
{
	printf("%hhu", i);
}


static void print_uint16(uint16_t i)
{
	printf("%hu", i);
}


static void print_int32(int32_t i)
{
	printf("%d", i);
}


static void print_uint32(uint32_t i)
{
	printf("%u", i);
}


/*
static void print_int64(int64_t i)
{
	printf("%" PRId64, i);
}
*/


static void print_uint64(uint64_t i)
{
	printf("%" PRIu64, i);
}



static void print_float(float f)
{
	printf("%f", f);
}




static void print_time(idmef_time_t *t)
{
	char buf[32];
	time_t _time;
	struct tm _tm;

	_time = idmef_time_get_sec(t);

	if ( ! localtime_r(&_time, &_tm) )
		return;

	if ( strftime(buf, sizeof (buf), "%H:%M:%S %d/%m/%Y", &_tm) == 0 )
		return;

	printf("%s (%u.%u)", buf, idmef_time_get_sec(t), idmef_time_get_usec(t));
}



/* print data as a string */

static void print_data(idmef_data_t *data)
{
	char buffer[512];

	if ( idmef_data_to_string(data, buffer, sizeof (buffer)) < 0 )
		return;

	printf("%s", buffer);
}



static void print_enum(const char *s, int i)
{
	printf("%s (%d)", s, i);
}


void idmef_additional_data_print(idmef_additional_data_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int i = idmef_additional_data_get_type(ptr);



		{
			print_indent();
			printf("type: ");
			print_enum(idmef_additional_data_type_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_additional_data_get_meaning(ptr);

		if ( field ) {
			print_indent();
			printf("meaning: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_data_t *field;

		field = idmef_additional_data_get_data(ptr);

		if ( field ) {
			print_indent();
			printf("data: ");
			print_data(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_reference_print(idmef_reference_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int i = idmef_reference_get_origin(ptr);



		{
			print_indent();
			printf("origin: ");
			print_enum(idmef_reference_origin_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_reference_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_reference_get_url(ptr);

		if ( field ) {
			print_indent();
			printf("url: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_reference_get_meaning(ptr);

		if ( field ) {
			print_indent();
			printf("meaning: ");
			print_string(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_classification_print(idmef_classification_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_classification_get_ident(ptr));
	printf("\n");

	{
		prelude_string_t *field;

		field = idmef_classification_get_text(ptr);

		if ( field ) {
			print_indent();
			printf("text: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_reference_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_classification_get_next_reference(ptr, elem)) ) {
			print_indent();

			printf("reference(%d):\n", cnt);
			idmef_reference_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_user_id_print(idmef_user_id_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_user_id_get_ident(ptr));
	printf("\n");

	{
		prelude_string_t *field;

		field = idmef_user_id_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		int i = idmef_user_id_get_type(ptr);



		{
			print_indent();
			printf("type: ");
			print_enum(idmef_user_id_type_to_string(i), i);
			printf("\n");
		}
        }

	{
		uint32_t *field;

		field = idmef_user_id_get_number(ptr);

		if ( field ) {
			print_indent();
			printf("number: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_user_print(idmef_user_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_user_get_ident(ptr));
	printf("\n");

	{
		int i = idmef_user_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_user_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		idmef_user_id_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_user_get_next_user_id(ptr, elem)) ) {
			print_indent();

			printf("user_id(%d):\n", cnt);
			idmef_user_id_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_address_print(idmef_address_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_address_get_ident(ptr));
	printf("\n");

	{
		int i = idmef_address_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_address_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_address_get_vlan_name(ptr);

		if ( field ) {
			print_indent();
			printf("vlan_name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		int32_t *field;

		field = idmef_address_get_vlan_num(ptr);

		if ( field ) {
			print_indent();
			printf("vlan_num: ");
			print_int32(*field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_address_get_address(ptr);

		if ( field ) {
			print_indent();
			printf("address: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_address_get_netmask(ptr);

		if ( field ) {
			print_indent();
			printf("netmask: ");
			print_string(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_process_print(idmef_process_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_process_get_ident(ptr));
	printf("\n");

	{
		prelude_string_t *field;

		field = idmef_process_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_process_get_pid(ptr);

		if ( field ) {
			print_indent();
			printf("pid: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_process_get_path(ptr);

		if ( field ) {
			print_indent();
			printf("path: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_process_get_next_arg(ptr, elem)) ) {
			print_indent();

			printf("arg(%d): ", cnt);
			print_string(elem);
			printf("\n");

			cnt++;
		}
	}

	{
		prelude_string_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_process_get_next_env(ptr, elem)) ) {
			print_indent();

			printf("env(%d): ", cnt);
			print_string(elem);
			printf("\n");

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_web_service_print(idmef_web_service_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_web_service_get_url(ptr);

		if ( field ) {
			print_indent();
			printf("url: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_web_service_get_cgi(ptr);

		if ( field ) {
			print_indent();
			printf("cgi: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_web_service_get_http_method(ptr);

		if ( field ) {
			print_indent();
			printf("http_method: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_web_service_get_next_arg(ptr, elem)) ) {
			print_indent();

			printf("arg(%d): ", cnt);
			print_string(elem);
			printf("\n");

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_snmp_service_print(idmef_snmp_service_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_oid(ptr);

		if ( field ) {
			print_indent();
			printf("oid: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_community(ptr);

		if ( field ) {
			print_indent();
			printf("community: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_security_name(ptr);

		if ( field ) {
			print_indent();
			printf("security_name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_context_name(ptr);

		if ( field ) {
			print_indent();
			printf("context_name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_context_engine_id(ptr);

		if ( field ) {
			print_indent();
			printf("context_engine_id: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_snmp_service_get_command(ptr);

		if ( field ) {
			print_indent();
			printf("command: ");
			print_string(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_service_print(idmef_service_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_service_get_ident(ptr));
	printf("\n");

	{
		uint8_t *field;

		field = idmef_service_get_ip_version(ptr);

		if ( field ) {
			print_indent();
			printf("ip_version: ");
			print_uint8(*field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_service_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		uint16_t *field;

		field = idmef_service_get_port(ptr);

		if ( field ) {
			print_indent();
			printf("port: ");
			print_uint16(*field);
			printf("\n");
		}
	}

	{
		uint8_t *field;

		field = idmef_service_get_iana_protocol_number(ptr);

		if ( field ) {
			print_indent();
			printf("iana_protocol_number: ");
			print_uint8(*field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_service_get_iana_protocol_name(ptr);

		if ( field ) {
			print_indent();
			printf("iana_protocol_name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_service_get_portlist(ptr);

		if ( field ) {
			print_indent();
			printf("portlist: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_service_get_protocol(ptr);

		if ( field ) {
			print_indent();
			printf("protocol: ");
			print_string(field);
			printf("\n");
		}
	}

	switch ( idmef_service_get_type(ptr) ) {
	case IDMEF_SERVICE_TYPE_WEB:
		print_indent();
		printf("web_service:\n");
		idmef_web_service_print(idmef_service_get_web_service(ptr));
		break;
 
	case IDMEF_SERVICE_TYPE_SNMP:
		print_indent();
		printf("snmp_service:\n");
		idmef_snmp_service_print(idmef_service_get_snmp_service(ptr));
		break;
 
	default:
		break;
	}

	indent -= 8;
}

void idmef_node_print(idmef_node_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_node_get_ident(ptr));
	printf("\n");

	{
		int i = idmef_node_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_node_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_node_get_location(ptr);

		if ( field ) {
			print_indent();
			printf("location: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_node_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_address_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_node_get_next_address(ptr, elem)) ) {
			print_indent();

			printf("address(%d):\n", cnt);
			idmef_address_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_source_print(idmef_source_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_source_get_ident(ptr));
	printf("\n");

	{
		int i = idmef_source_get_spoofed(ptr);



		{
			print_indent();
			printf("spoofed: ");
			print_enum(idmef_source_spoofed_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_source_get_interface(ptr);

		if ( field ) {
			print_indent();
			printf("interface: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_node_t *field;

		field = idmef_source_get_node(ptr);

		if ( field ) {
			print_indent();
			printf("node:\n");
			idmef_node_print(field);
		}
	}	

	{
		idmef_user_t *field;

		field = idmef_source_get_user(ptr);

		if ( field ) {
			print_indent();
			printf("user:\n");
			idmef_user_print(field);
		}
	}	

	{
		idmef_process_t *field;

		field = idmef_source_get_process(ptr);

		if ( field ) {
			print_indent();
			printf("process:\n");
			idmef_process_print(field);
		}
	}	

	{
		idmef_service_t *field;

		field = idmef_source_get_service(ptr);

		if ( field ) {
			print_indent();
			printf("service:\n");
			idmef_service_print(field);
		}
	}	

	indent -= 8;
}

void idmef_file_access_print(idmef_file_access_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		idmef_user_id_t *field;

		field = idmef_file_access_get_user_id(ptr);

		if ( field ) {
			print_indent();
			printf("user_id:\n");
			idmef_user_id_print(field);
		}
	}	

	{
		prelude_string_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_file_access_get_next_permission(ptr, elem)) ) {
			print_indent();

			printf("permission(%d): ", cnt);
			print_string(elem);
			printf("\n");

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_inode_print(idmef_inode_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		idmef_time_t *field;

		field = idmef_inode_get_change_time(ptr);

		if ( field ) {
			print_indent();
			printf("change_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_inode_get_number(ptr);

		if ( field ) {
			print_indent();
			printf("number: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_inode_get_major_device(ptr);

		if ( field ) {
			print_indent();
			printf("major_device: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_inode_get_minor_device(ptr);

		if ( field ) {
			print_indent();
			printf("minor_device: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_inode_get_c_major_device(ptr);

		if ( field ) {
			print_indent();
			printf("c_major_device: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_inode_get_c_minor_device(ptr);

		if ( field ) {
			print_indent();
			printf("c_minor_device: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_checksum_print(idmef_checksum_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_checksum_get_value(ptr);

		if ( field ) {
			print_indent();
			printf("value: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_checksum_get_key(ptr);

		if ( field ) {
			print_indent();
			printf("key: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		int i = idmef_checksum_get_algorithm(ptr);



		{
			print_indent();
			printf("algorithm: ");
			print_enum(idmef_checksum_algorithm_to_string(i), i);
			printf("\n");
		}
        }

	indent -= 8;
}

void idmef_file_print(idmef_file_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_file_get_ident(ptr));
	printf("\n");

	{
		prelude_string_t *field;

		field = idmef_file_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_file_get_path(ptr);

		if ( field ) {
			print_indent();
			printf("path: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_time_t *field;

		field = idmef_file_get_create_time(ptr);

		if ( field ) {
			print_indent();
			printf("create_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_time_t *field;

		field = idmef_file_get_modify_time(ptr);

		if ( field ) {
			print_indent();
			printf("modify_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_time_t *field;

		field = idmef_file_get_access_time(ptr);

		if ( field ) {
			print_indent();
			printf("access_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		uint64_t *field;

		field = idmef_file_get_data_size(ptr);

		if ( field ) {
			print_indent();
			printf("data_size: ");
			print_uint64(*field);
			printf("\n");
		}
	}

	{
		uint64_t *field;

		field = idmef_file_get_disk_size(ptr);

		if ( field ) {
			print_indent();
			printf("disk_size: ");
			print_uint64(*field);
			printf("\n");
		}
	}

	{
		idmef_file_access_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_file_get_next_file_access(ptr, elem)) ) {
			print_indent();

			printf("file_access(%d):\n", cnt);
			idmef_file_access_print(elem);

			cnt++;
		}
	}

	{
		idmef_linkage_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_file_get_next_linkage(ptr, elem)) ) {
			print_indent();

			printf("linkage(%d):\n", cnt);
			idmef_linkage_print(elem);

			cnt++;
		}
	}

	{
		idmef_inode_t *field;

		field = idmef_file_get_inode(ptr);

		if ( field ) {
			print_indent();
			printf("inode:\n");
			idmef_inode_print(field);
		}
	}	

	{
		idmef_checksum_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_file_get_next_checksum(ptr, elem)) ) {
			print_indent();

			printf("checksum(%d):\n", cnt);
			idmef_checksum_print(elem);

			cnt++;
		}
	}

	{
		int i = idmef_file_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_file_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		int *i = idmef_file_get_fstype(ptr);


		if ( i )


		{
			print_indent();
			printf("fstype: ");
			print_enum(idmef_file_fstype_to_string(*i), *i);
			printf("\n");
		}
        }

	indent -= 8;
}

void idmef_linkage_print(idmef_linkage_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int i = idmef_linkage_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_linkage_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_linkage_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_linkage_get_path(ptr);

		if ( field ) {
			print_indent();
			printf("path: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_file_t *field;

		field = idmef_linkage_get_file(ptr);

		if ( field ) {
			print_indent();
			printf("file:\n");
			idmef_file_print(field);
		}
	}	

	indent -= 8;
}

void idmef_target_print(idmef_target_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("ident: ");
	print_uint64(idmef_target_get_ident(ptr));
	printf("\n");

	{
		int i = idmef_target_get_decoy(ptr);



		{
			print_indent();
			printf("decoy: ");
			print_enum(idmef_target_decoy_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_target_get_interface(ptr);

		if ( field ) {
			print_indent();
			printf("interface: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_node_t *field;

		field = idmef_target_get_node(ptr);

		if ( field ) {
			print_indent();
			printf("node:\n");
			idmef_node_print(field);
		}
	}	

	{
		idmef_user_t *field;

		field = idmef_target_get_user(ptr);

		if ( field ) {
			print_indent();
			printf("user:\n");
			idmef_user_print(field);
		}
	}	

	{
		idmef_process_t *field;

		field = idmef_target_get_process(ptr);

		if ( field ) {
			print_indent();
			printf("process:\n");
			idmef_process_print(field);
		}
	}	

	{
		idmef_service_t *field;

		field = idmef_target_get_service(ptr);

		if ( field ) {
			print_indent();
			printf("service:\n");
			idmef_service_print(field);
		}
	}	

	{
		idmef_file_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_target_get_next_file(ptr, elem)) ) {
			print_indent();

			printf("file(%d):\n", cnt);
			idmef_file_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_analyzer_print(idmef_analyzer_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("analyzerid: ");
	print_uint64(idmef_analyzer_get_analyzerid(ptr));
	printf("\n");

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_manufacturer(ptr);

		if ( field ) {
			print_indent();
			printf("manufacturer: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_model(ptr);

		if ( field ) {
			print_indent();
			printf("model: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_version(ptr);

		if ( field ) {
			print_indent();
			printf("version: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_class(ptr);

		if ( field ) {
			print_indent();
			printf("class: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_ostype(ptr);

		if ( field ) {
			print_indent();
			printf("ostype: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_analyzer_get_osversion(ptr);

		if ( field ) {
			print_indent();
			printf("osversion: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_node_t *field;

		field = idmef_analyzer_get_node(ptr);

		if ( field ) {
			print_indent();
			printf("node:\n");
			idmef_node_print(field);
		}
	}	

	{
		idmef_process_t *field;

		field = idmef_analyzer_get_process(ptr);

		if ( field ) {
			print_indent();
			printf("process:\n");
			idmef_process_print(field);
		}
	}	

	{
		idmef_analyzer_t *field;

		field = idmef_analyzer_get_analyzer(ptr);

		if ( field ) {
			print_indent();
			printf("analyzer:\n");
			idmef_analyzer_print(field);
		}
	}	

	indent -= 8;
}

void idmef_alertident_print(idmef_alertident_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("alertident: ");
	print_uint64(idmef_alertident_get_alertident(ptr));
	printf("\n");

	{
		uint64_t *field;

		field = idmef_alertident_get_analyzerid(ptr);

		if ( field ) {
			print_indent();
			printf("analyzerid: ");
			print_uint64(*field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_impact_print(idmef_impact_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int *i = idmef_impact_get_severity(ptr);


		if ( i )


		{
			print_indent();
			printf("severity: ");
			print_enum(idmef_impact_severity_to_string(*i), *i);
			printf("\n");
		}
        }

	{
		int *i = idmef_impact_get_completion(ptr);


		if ( i )


		{
			print_indent();
			printf("completion: ");
			print_enum(idmef_impact_completion_to_string(*i), *i);
			printf("\n");
		}
        }

	{
		int i = idmef_impact_get_type(ptr);



		{
			print_indent();
			printf("type: ");
			print_enum(idmef_impact_type_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_impact_get_description(ptr);

		if ( field ) {
			print_indent();
			printf("description: ");
			print_string(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_action_print(idmef_action_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int i = idmef_action_get_category(ptr);



		{
			print_indent();
			printf("category: ");
			print_enum(idmef_action_category_to_string(i), i);
			printf("\n");
		}
        }

	{
		prelude_string_t *field;

		field = idmef_action_get_description(ptr);

		if ( field ) {
			print_indent();
			printf("description: ");
			print_string(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_confidence_print(idmef_confidence_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		int i = idmef_confidence_get_rating(ptr);



		{
			print_indent();
			printf("rating: ");
			print_enum(idmef_confidence_rating_to_string(i), i);
			printf("\n");
		}
        }

	{
		float *field;

		field = idmef_confidence_get_confidence(ptr);

		if ( field ) {
			print_indent();
			printf("confidence: ");
			print_float(*field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_assessment_print(idmef_assessment_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		idmef_impact_t *field;

		field = idmef_assessment_get_impact(ptr);

		if ( field ) {
			print_indent();
			printf("impact:\n");
			idmef_impact_print(field);
		}
	}	

	{
		idmef_action_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_assessment_get_next_action(ptr, elem)) ) {
			print_indent();

			printf("action(%d):\n", cnt);
			idmef_action_print(elem);

			cnt++;
		}
	}

	{
		idmef_confidence_t *field;

		field = idmef_assessment_get_confidence(ptr);

		if ( field ) {
			print_indent();
			printf("confidence:\n");
			idmef_confidence_print(field);
		}
	}	

	indent -= 8;
}

void idmef_tool_alert_print(idmef_tool_alert_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_tool_alert_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		prelude_string_t *field;

		field = idmef_tool_alert_get_command(ptr);

		if ( field ) {
			print_indent();
			printf("command: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_alertident_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_tool_alert_get_next_alertident(ptr, elem)) ) {
			print_indent();

			printf("alertident(%d):\n", cnt);
			idmef_alertident_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_correlation_alert_print(idmef_correlation_alert_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_correlation_alert_get_name(ptr);

		if ( field ) {
			print_indent();
			printf("name: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		idmef_alertident_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_correlation_alert_get_next_alertident(ptr, elem)) ) {
			print_indent();

			printf("alertident(%d):\n", cnt);
			idmef_alertident_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_overflow_alert_print(idmef_overflow_alert_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_overflow_alert_get_program(ptr);

		if ( field ) {
			print_indent();
			printf("program: ");
			print_string(field);
			printf("\n");
		}
	}

	{
		uint32_t *field;

		field = idmef_overflow_alert_get_size(ptr);

		if ( field ) {
			print_indent();
			printf("size: ");
			print_uint32(*field);
			printf("\n");
		}
	}

	{
		idmef_data_t *field;

		field = idmef_overflow_alert_get_buffer(ptr);

		if ( field ) {
			print_indent();
			printf("buffer: ");
			print_data(field);
			printf("\n");
		}
	}

	indent -= 8;
}

void idmef_alert_print(idmef_alert_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("messageid: ");
	print_uint64(idmef_alert_get_messageid(ptr));
	printf("\n");

	{
		idmef_analyzer_t *field;

		field = idmef_alert_get_analyzer(ptr);

		if ( field ) {
			print_indent();
			printf("analyzer:\n");
			idmef_analyzer_print(field);
		}
	}	

	{
		idmef_time_t *field;

		field = idmef_alert_get_create_time(ptr);

		if ( field ) {
			print_indent();
			printf("create_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_classification_t *field;

		field = idmef_alert_get_classification(ptr);

		if ( field ) {
			print_indent();
			printf("classification:\n");
			idmef_classification_print(field);
		}
	}	

	{
		idmef_time_t *field;

		field = idmef_alert_get_detect_time(ptr);

		if ( field ) {
			print_indent();
			printf("detect_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_time_t *field;

		field = idmef_alert_get_analyzer_time(ptr);

		if ( field ) {
			print_indent();
			printf("analyzer_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_source_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_alert_get_next_source(ptr, elem)) ) {
			print_indent();

			printf("source(%d):\n", cnt);
			idmef_source_print(elem);

			cnt++;
		}
	}

	{
		idmef_target_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_alert_get_next_target(ptr, elem)) ) {
			print_indent();

			printf("target(%d):\n", cnt);
			idmef_target_print(elem);

			cnt++;
		}
	}

	{
		idmef_assessment_t *field;

		field = idmef_alert_get_assessment(ptr);

		if ( field ) {
			print_indent();
			printf("assessment:\n");
			idmef_assessment_print(field);
		}
	}	

	{
		idmef_additional_data_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_alert_get_next_additional_data(ptr, elem)) ) {
			print_indent();

			printf("additional_data(%d):\n", cnt);
			idmef_additional_data_print(elem);

			cnt++;
		}
	}

	switch ( idmef_alert_get_type(ptr) ) {
	case IDMEF_ALERT_TYPE_TOOL:
		print_indent();
		printf("tool_alert:\n");
		idmef_tool_alert_print(idmef_alert_get_tool_alert(ptr));
		break;
 
	case IDMEF_ALERT_TYPE_CORRELATION:
		print_indent();
		printf("correlation_alert:\n");
		idmef_correlation_alert_print(idmef_alert_get_correlation_alert(ptr));
		break;
 
	case IDMEF_ALERT_TYPE_OVERFLOW:
		print_indent();
		printf("overflow_alert:\n");
		idmef_overflow_alert_print(idmef_alert_get_overflow_alert(ptr));
		break;
 
	default:
		break;
	}

	indent -= 8;
}

void idmef_heartbeat_print(idmef_heartbeat_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	print_indent();
	printf("messageid: ");
	print_uint64(idmef_heartbeat_get_messageid(ptr));
	printf("\n");

	{
		idmef_analyzer_t *field;

		field = idmef_heartbeat_get_analyzer(ptr);

		if ( field ) {
			print_indent();
			printf("analyzer:\n");
			idmef_analyzer_print(field);
		}
	}	

	{
		idmef_time_t *field;

		field = idmef_heartbeat_get_create_time(ptr);

		if ( field ) {
			print_indent();
			printf("create_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_time_t *field;

		field = idmef_heartbeat_get_analyzer_time(ptr);

		if ( field ) {
			print_indent();
			printf("analyzer_time: ");
			print_time(field);
			printf("\n");
		}
	}

	{
		idmef_additional_data_t *elem = NULL;
		int cnt = 0;

		while ( (elem = idmef_heartbeat_get_next_additional_data(ptr, elem)) ) {
			print_indent();

			printf("additional_data(%d):\n", cnt);
			idmef_additional_data_print(elem);

			cnt++;
		}
	}

	indent -= 8;
}

void idmef_message_print(idmef_message_t *ptr)
{
	if ( ! ptr )
		return;

	indent += 8;

	{
		prelude_string_t *field;

		field = idmef_message_get_version(ptr);

		if ( field ) {
			print_indent();
			printf("version: ");
			print_string(field);
			printf("\n");
		}
	}

	switch ( idmef_message_get_type(ptr) ) {
	case IDMEF_MESSAGE_TYPE_ALERT:
		print_indent();
		printf("alert:\n");
		idmef_alert_print(idmef_message_get_alert(ptr));
		break;
 
	case IDMEF_MESSAGE_TYPE_HEARTBEAT:
		print_indent();
		printf("heartbeat:\n");
		idmef_heartbeat_print(idmef_message_get_heartbeat(ptr));
		break;
 
	default:
		break;
	}

	indent -= 8;
}
