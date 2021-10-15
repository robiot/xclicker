import React from "react";


export function Release({ children, latestRelease, latestReleaseAssets }) {
    return (
        <div className="mt-10 p-3 bg-gradient-gray-less rounded-md">
        <div className="font-semibold text-2xl mb-2">
          <a href={latestRelease.html_url} target="_blank">Latest {latestRelease.tag_name}</a>
        </div>
        <div className="bg-white h-1"></div>
        <div className="mt-2 flex flex-col-reverse divide-y divide-y-reverse ">
          { children }
        </div>
      </div>
    )
}