
/*
 * Copyright (c) 2006-2018 RT-Thread Development Team. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "mbedtls/certs.h"


#if 0 
const char mbedtls_root_certificate[] = 
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\r\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\r\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\r\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\r\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\r\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\r\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\r\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\r\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\r\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\r\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\r\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\r\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\r\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\r\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\r\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\r\n" \
"-----END CERTIFICATE-----\r\n" \

;
#endif


const char mbedtls_root_certificate[] = 
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDyTCCArGgAwIBAgIUEcab+nIQ/H2moSCO9fQjcdEqwGUwDQYJKoZIhvcNAQEL\r\n" \
"BQAwdDELMAkGA1UEBhMCemgxCzAJBgNVBAgMAnpqMQswCQYDVQQHDAJuYjENMAsG\r\n" \
"A1UECgwEZXN0bzENMAsGA1UECwwEcXlzajELMAkGA1UEAwwCaGoxIDAeBgkqhkiG\r\n" \
"9w0BCQEWEWh1YW5namlhbkBlc3RvLmNuMB4XDTE5MTExMTA2NDYxMloXDTE5MTIx\r\n" \
"MTA2NDYxMlowdDELMAkGA1UEBhMCemgxCzAJBgNVBAgMAnpqMQswCQYDVQQHDAJu\r\n" \
"YjENMAsGA1UECgwEZXN0bzENMAsGA1UECwwEcXlzajELMAkGA1UEAwwCaGoxIDAe\r\n" \
"BgkqhkiG9w0BCQEWEWh1YW5namlhbkBlc3RvLmNuMIIBIjANBgkqhkiG9w0BAQEF\r\n" \
"AAOCAQ8AMIIBCgKCAQEAw6irffv3cYgScC57KdB3YrVlmpUIvauuxkR7un5Ia8ML\r\n" \
"+21JxZh5dW5Xop6aXUgXJATvF4SK4bF8L1j9FylSx3IyPOAMyT5bQsZ7aF3Va+Sw\r\n" \
"dLHM5enNcQWtnVqlB4noTwcX+JdkAXVuPGQV0URErNmNHrz6q3wBwXpupYv8zXSj\r\n" \
"BtwAcqbBllZQsWWYbs1PwXynAS9JOfR1pqnVSSIxdLZfNZEJYPWbMhBkg2l2WJ0f\r\n" \
"nNkSv2L+Jc6D8ATmtMq8/F+s0jsSTRf54WAk3sfpN8K5sW7yFAmoLfNgmu9m3gF+\r\n" \
"3dauOGVFSXS12sjmhH7eLh6fF5QZuOixC0Jq7fG9bwIDAQABo1MwUTAdBgNVHQ4E\r\n" \
"FgQUr/cPfzGtL0WLX/cyfwNYSv1Zr0wwHwYDVR0jBBgwFoAUr/cPfzGtL0WLX/cy\r\n" \
"fwNYSv1Zr0wwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAMhfs\r\n" \
"4XJdE8a9f6cCJ189XMEZv0skYylC41CK+nm65Cru2aljMLPfJSlsTSMNp7sYwB7Z\r\n" \
"zme1xPouBD0jYKNChiWnQMeTi3jDzQUDh6zZvx3JNraNJVWlbUoXsswyDFjD1uKl\r\n" \
"QaGeN2qcTZaoamN4+On1gi6QivTUKM5aFZskRBX1bc/jZNcNlTC5p79NC2nFTtTu\r\n" \
"/kylP6sw37hhacjfW2sP8vdfmeOyDHAIIjFjIxPil49ECis+VXcCp51L1Si+kqEQ\r\n" \
"WzEfRk2Ok++UBksRl3Xd+Hse83qC8rSRiDkgHcXKr5FySwyqPVWtjXod9rmhvUfU\r\n" \
"ohY8k6YxwOOShRNlbw==\r\n" \
"-----END CERTIFICATE-----\r\n" \

;

const size_t mbedtls_root_certificate_len = sizeof(mbedtls_root_certificate);

