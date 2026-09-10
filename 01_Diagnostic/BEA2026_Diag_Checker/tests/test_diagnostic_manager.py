from __future__ import annotations

import logging

import pytest
import serial

from bea_diag_checker.core import (
    DiagnosticManager,
    DiagnosticServiceRegistrationError,
    InvalidDiagnosticRequestError,
    SerialManager,
    UnknownDiagnosticServiceError,
)
from bea_diag_checker.config import END_OF_FRAME, START_OF_FRAME
from bea_diag_checker.core.diagnostic_manager import DiagnosticRequestFromUI


class FakeSerial:
    def __init__(self, **_kwargs: object) -> None:
        self.is_open = True
        self.writes: list[bytes] = []

    def close(self) -> None:
        self.is_open = False

    def flush(self) -> None:
        if not self.is_open:
            raise serial.SerialException("closed")

    def readline(self) -> bytes:
        return b""

    def write(self, data: bytes) -> int:
        if not self.is_open:
            raise serial.SerialException("closed")
        self.writes.append(data)
        return len(data)


class FakeFactory:
    def __init__(self) -> None:
        self.connection = FakeSerial()

    def __call__(self, **_kwargs: object) -> FakeSerial:
        return self.connection


def make_manager() -> tuple[DiagnosticManager, FakeFactory]:
    logger = logging.getLogger("test_diagnostic_manager")
    logger.handlers.clear()
    logger.addHandler(logging.NullHandler())
    factory = FakeFactory()
    serial_manager = SerialManager(logger, serial_factory=factory)
    return DiagnosticManager(serial_manager, logger), factory


def test_builtin_services_are_discovered_from_service_id_filenames() -> None:
    manager, _ = make_manager()

    assert manager.list_services() == {
        "0x22": "read_DID",
        "0x27": "security_access",
        "0x2E": "write_DID",
    }


def test_read_did_process_builds_binary_uds_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    payload = manager.process("0x22", "0xF180")

    assert payload == START_OF_FRAME + b"\x22\xF1\x80" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_read_did_process_builds_string_uds_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    payload = manager.process("0x22", "0xF180", data_format="String")

    assert payload == START_OF_FRAME + b"22F180" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_security_access_process_builds_request_seed_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    payload = manager.process("0x27", {"sub_function": "0x01"})

    assert payload == START_OF_FRAME + b"\x27\x01" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_security_access_process_builds_send_key_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    payload = manager.process(
        "0x27",
        {"sub_function": 0x02, "key": "AA BB CC DD"},
    )

    assert payload == START_OF_FRAME + b"\x27\x02\xAA\xBB\xCC\xDD" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_security_access_process_supports_string_format() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    payload = manager.process(
        "0x27",
        {"sub_function": "0x01"},
        data_format="String",
    )

    assert payload == START_OF_FRAME + b"2701" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_read_did_rejects_invalid_did() -> None:
    manager, _ = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    with pytest.raises(InvalidDiagnosticRequestError, match="DID"):
        manager.process("0x22", "not-a-did")

    manager.serial_manager.disconnect(announce=False)


def test_unknown_service_is_rejected() -> None:
    """0xFF is not a registered service - must raise UnknownDiagnosticServiceError."""
    manager, _ = make_manager()

    with pytest.raises(UnknownDiagnosticServiceError, match="Unknown diagnostic service"):
        manager.process("0xFF", "F180")


def test_service_registration_requires_process_function() -> None:
    manager, _ = make_manager()

    class InvalidService:
        SERVICE_ID = 0x99
        SERVICE_NAME = "invalid"

    with pytest.raises(DiagnosticServiceRegistrationError, match="process"):
        manager.register_service(InvalidService())


# ── Write DID (0x2E) tests ────────────────────────────────────────────────────

def test_write_did_process_builds_binary_uds_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    request = DiagnosticRequestFromUI(DID="0xF180", Data=b"\xAB\xCD", DataLength=2)
    payload = manager.process("0x2E", request)

    assert payload == START_OF_FRAME + b"\x2E\xF1\x80\xAB\xCD" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_write_did_process_builds_string_uds_request() -> None:
    manager, factory = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    request = DiagnosticRequestFromUI(DID="0xF180", Data=b"\xAB\xCD", DataLength=2)
    payload = manager.process("0x2E", request, data_format="String")

    assert payload == START_OF_FRAME + b"2EF180ABCD" + END_OF_FRAME
    assert factory.connection.writes == [payload]
    manager.serial_manager.disconnect(announce=False)


def test_write_did_rejects_invalid_did() -> None:
    manager, _ = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    request = DiagnosticRequestFromUI(DID="not-a-did", Data=b"\xAB", DataLength=1)
    with pytest.raises(InvalidDiagnosticRequestError, match="DID"):
        manager.process("0x2E", request)

    manager.serial_manager.disconnect(announce=False)


def test_write_did_rejects_data_length_mismatch() -> None:
    """DataLength=3 nhưng Data chỉ có 2 bytes -> phải raise lỗi."""
    manager, _ = make_manager()
    manager.serial_manager.connect("COM7", 115200)

    request = DiagnosticRequestFromUI(DID="0xF180", Data=b"\xAB\xCD", DataLength=3)
    with pytest.raises(InvalidDiagnosticRequestError):
        manager.process("0x2E", request)

    manager.serial_manager.disconnect(announce=False)
