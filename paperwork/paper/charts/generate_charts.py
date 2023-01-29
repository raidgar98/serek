from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime
from json import load
from pathlib import Path
from typing import Dict, List

import matplotlib.pyplot as plt

datetime_month = lambda x: datetime.strptime(x, "%Y-%m")

root_dir = Path(__file__).parent.absolute()
output_dir = root_dir / Path("output_with_charts_as_images")
input_dir = root_dir / Path("data")


@dataclass
class ChartInfo:
    data_source: str
    columns: Dict[str, str]
    series: List[str]
    domain: str
    title: str
    source: str
    separator: str
    domain_label: str
    counter_domain_label: str

    @staticmethod
    def from_json(json_path: Path) -> ChartInfo:
        with json_path.open() as in_file:
            return ChartInfo(**load(in_file))

    def save_figure(self, dest_path: Path) -> None:
        column_keys: List[str] = list(self.columns.keys())
        column_values: List[type] = [eval(x) for x in list(self.columns.values())]
        csv = {column_name: [] for column_name in column_keys}

        with (root_dir / self.data_source).open("rt") as csv_file:
            for line in csv_file:
                items = line.strip("\n").split(self.separator)
                assert len(items) == len(column_keys)
                for i, item in enumerate(items):
                    dst_col_name: str = column_keys[i]
                    dst_type: type = column_values[i]
                    csv[dst_col_name].append(dst_type(item))
        domain = csv.pop(self.domain)

        plt.cla()
        plt.clf()

        fig, axes = plt.subplots()
        axes.tick_params("x", direction="in")
        axes.tick_params("y", direction="in")

        plt.ylabel(self.counter_domain_label)
        plt.xlabel(self.domain_label)
        fig.subplots_adjust(top=0.85)

        # A4: 21 x 29.7cm
        # margins: 2.5cm each
        # binding: 1cm
        # final size width = (21 - (2*2.5) - 1) cm
        convert_cm_to_inches = lambda cm: cm / 2.54
        fig.set_figwidth(convert_cm_to_inches(15))
        fig.set_figheight(convert_cm_to_inches(10))
        fig.set_tight_layout('w_pad')
        fig.set_dpi(600)

        for label, values in csv.items():
            plt.plot(domain, values, label=label)
            plt.legend(loc="best")
        plt.savefig(dest_path)

if output_dir.exists():
    for file in output_dir.glob("*"):
        file = Path(file).unlink(False)
    output_dir.rmdir()
output_dir.mkdir()

for dir in input_dir.glob("*"):
    dir: Path = dir
    if dir.is_dir():
        info_json = dir / "info.json"
        if info_json.exists():
            info = ChartInfo.from_json(info_json)
            info.save_figure(output_dir / f"{dir.parts[-1]}.png")
