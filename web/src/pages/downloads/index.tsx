import Link from "next/link";
import { useEffect, useRef, useState } from "react";
import Layout from "../../components/Layout";
import Nav from "../../components/Nav";
import { Release } from "../../components/Release";

function formatBytes(bytes, decimals = 2) {
  if (bytes === 0) return "0 Bytes";

  const k = 1024;
  const dm = decimals < 0 ? 0 : decimals;
  const sizes = ["Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"];
  const i = Math.floor(Math.log(bytes) / Math.log(k));

  return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + " " + sizes[i];
}

export default function Home() {
  const [latestRelease, setLatestRelease] = useState<any>(JSON);
  const [latestReleaseAssets, setLatestReleaseAssets] = useState<any>(Array);

  //const [allReleases, setAllReleases] = useState<any>(Array);

  //const [releases, setReleases] = useState(Array);

  useEffect(() => {
    async function grab() {
      const latest = await fetch(
        "https://api.github.com/repos/robiot/XClicker/releases/latest"
      );
      const latestjson = await latest.json();
      setLatestRelease(latestjson);
      setLatestReleaseAssets(await latestjson.assets);
    }

    grab();
  }, []);
  return (
    <Layout title="Autoclicker For Linux">
      <div className="bg-gradient-gray h-screen">
        <Nav />

        <div className="container-xl mx-auto px-10 mt-10">
          <div className="font-semibold text-3xl">Downloads</div>
          <div>
            If you are unsure what to download, check out the{" "}
            <a
              href="https://github.com/robiot/XClicker/wiki/Installation"
              className="text-blue-500 hover:underline"
              target="_blank"
            >
              Installation Guide
            </a>
          </div>

          <Release latestRelease={latestRelease}>
            {latestReleaseAssets.map((asset, index) => (
              <div key={index} className="p-2">
                <div className="flex justify-between overflow-hidden">
                  <a
                    className="text-blue-400 hover:underline"
                    href={asset.browser_download_url}
                  >
                    {asset.name}
                  </a>
                  <div className="text-gray-200 hidden md:block">
                    {formatBytes(asset.size)}
                  </div>
                </div>
              </div>
            ))}
          </Release>

          <div className="text-xl mt-5">
            All older versions is available for download at the
            <a
              href="https://github.com/robiot/XClicker/releases"
              className="text-blue-500 hover:underline"
              target="_blank"
            >
              {" "}
              Github Releases Page
            </a>
          </div>
        </div>
      </div>
    </Layout>
  );
}
