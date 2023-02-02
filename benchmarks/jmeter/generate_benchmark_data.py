from dataclasses import dataclass, field
from datetime import datetime
import json
from random import random, randint
from typing import Final

BIG_NUM: Final[int] = (2**31) - 1
ARRAY_SIZE: Final[int] = 1_000
OBJECTS: Final[int] = 10_000

def random_float() -> float:
	return (random() - 0.5) * BIG_NUM

def random_bool() -> bool:
	return random() >= 0.5

def random_int() -> int:
	return randint(-BIG_NUM, BIG_NUM)

def random_unsigned_int() -> int:
	return randint(0, BIG_NUM)

@dataclass
class A:
	a1: bool = field(default_factory=random_bool)
	a2: int = field(default_factory=random_int)
	a3: int = field(default_factory=random_unsigned_int)
	a4: float = field(default_factory=random_float)
	a5: float = field(default_factory=random_float)

@dataclass
class B:
	b1: list = field(default_factory=lambda: [random_int() for _ in range(ARRAY_SIZE)])
	b2: list = field(default_factory=lambda: [random_float() for _ in range(ARRAY_SIZE)])
	b3: list = field(default_factory=lambda: [A() for _ in range(ARRAY_SIZE)])

@dataclass
class C(A, B):
	c1: A = field(default_factory=A)
	c2: B = field(default_factory=B)


if __name__ == "__main__":
	with open("data.csv", "wt") as out_file:
		start = datetime.now()
		for i in range(OBJECTS):
			print(f"generating object {i+1}/{OBJECTS}", end="\r")
			out_file.write(json.dumps(C(), default=vars) + "\n")

print(f"finished generation of {OBJECTS} objects in {(datetime.now() - start).total_seconds() :.2f} seconds")
