import Skeleton from "react-loading-skeleton";

export function Release({ children, latestRelease, isLoading }) {
  return (
    <div className="mt-10 p-3 bg-gray-600 bg-opacity-30 rounded-md">
      <div className="font-semibold text-2xl mb-2">
        <a
          href={latestRelease.html_url}
          target="_blank"
          rel="noopener noreferrer"
        >
          {isLoading ? (
            <Skeleton />
          ) : (
            <div>Latest {latestRelease.tag_name}</div>
          )}
        </a>
      </div>
      {children}
    </div>
  );
}
